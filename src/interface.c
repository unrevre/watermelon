#include "interface.h"

#include "debug.h"
#include "generate.h"
#include "magics.h"
#include "masks.h"
#include "memory.h"
#include "position.h"
#include "state.h"
#include "utilities.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define flag(itf, flag) (itf->flags & (flag))

/*!
 * wmprintf
 * @ wrapper for printf, discarding extra argument
 */

int wmprintf(WINDOW* w __attribute__((unused)),
             uint64_t clear __attribute__((unused)),
             char const* fmt, va_list args) {
   return vprintf(fmt, args);
}

/*!
 * wmprintw
 * @ wrapper for mvwprintw
 */

int wmprintw(WINDOW* w, uint64_t clear, char const* fmt, va_list args) {
   if (clear) { wmove(w, 0, 0); }

   return vwprintw(w, fmt, args);
}

/*!
 * wmprint
 * @ wrapper for print functions
 */

void wmprint(interface_t* itf, WINDOW* w, int64_t clear, char const* fmt,
             ...) {
   va_list args;
   va_start(args, fmt);
   itf->print(w, clear, fmt, args);
   va_end(args);

   fflush(stdout);
}

/*!
 * refresh_windows
 * @ refresh window contents
 */

void refresh_windows(interface_t* itf, int64_t count, ...) {
   if (flag(itf, ITF_CURSES)) {
      wmove(itf->win_state, itf->y, itf->x);

      va_list args;
      va_start(args, count);
      for (int64_t i = 0; i < count; ++i) {
         wnoutrefresh(va_arg(args, WINDOW*)); }
      va_end(args);

      doupdate();
   }
}

void init_interface(interface_t* itf, uint64_t flags) {
   itf->flags = flags;
   itf->print = flag(itf, ITF_CURSES) ? wmprintw : wmprintf;
   itf->x = 1;
   itf->y = 0;
   itf->index = -1;

   itf->info = malloc(sizeof(debug_t));
   init_debug(itf->info);

   if (flag(itf, ITF_CURSES)) {
      initscr();
      cbreak();
      noecho();

      itf->win_fen = newwin(1, 79, 0, 0);
      itf->border_info = newwin(20, 48, 17, 1);
      itf->win_info = newwin(18, 44, 18, 3);
      itf->border_state = newwin(14, 23, 2, 1);
      itf->win_state = newwin(10, 20, 4, 3);

      scrollok(itf->win_info, TRUE);
      box(itf->border_state, 0, 0);
      box(itf->border_info, 0, 0);

      wnoutrefresh(stdscr);
      wnoutrefresh(itf->win_fen);
      wnoutrefresh(itf->border_info);
      wnoutrefresh(itf->border_state);

      doupdate();
   }
}

void close_interface(interface_t* itf) {
   wmprint_info(itf, " - exit -\n");
   refresh_windows(itf, 1, itf->win_info);

   if (flag(itf, ITF_CURSES)) {
      getch();
      endwin();
   }

   free_debug(itf->info);
   free(itf);
}

void refresh_all(interface_t* itf) {
   refresh_windows(itf, 3, itf->win_fen, itf->win_info, itf->win_state);
}

void wmprint_state(interface_t* itf) {
   if (flag(itf, ITF_QUIET) && !flag(itf, ITF_CURSES)) { return; }

   wmprint(itf, itf->win_fen, 1, "%s\n", info_fen(itf->info));
   wmprint(itf, itf->win_state, 1, "%s", info_game_state(itf->info));
}

void wmprint_search(interface_t* itf, move_t move) {
   wmprint(itf, itf->win_info, 0, "%s\n", info_move(itf->info, move));
   if (flag(itf, ITF_QUIET)) { return; }

   wmprint_info(itf, "\n");
   char** pv = info_principal_variation(itf->info);
   for (int64_t i = 0; i < PLYLIMIT && pv[i][0]; ++i)
      wmprint_info(itf, "%s", pv[i]);
   wmprint_info(itf, "\n");
}

void wmprint_info(interface_t* itf, char const* fmt, ...) {
   if (flag(itf, ITF_QUIET)) { return; }

   va_list args;
   va_start(args, fmt);
   itf->print(itf->win_info, 0, fmt, args);
   va_end(args);
}

/*!
 * is_index_movable
 * @ test if piece at index is on side to move
 */

uint32_t is_index_movable(int64_t index) {
   return (trunk.board[index] != empty
      && s(trunk.board[index]) == trunk.side);
}

/*!
 * move_for_indices
 * @ return move if indices constitute a valid move
 */

move_t move_for_indices(uint32_t from, uint32_t to) {
   if (is_index_movable(to)) { return (move_t){0}; }

   int64_t side = trunk.side;
   __uint128_t tmask = PMASK[to];

   int32_t fdiff = (from - OFFSET) % WIDTH - (to - OFFSET) % WIDTH;
   int32_t fdabs = abs(fdiff);
   int32_t rdiff = (from - OFFSET) / WIDTH - (to - OFFSET) / WIDTH;
   int32_t rdabs = abs(rdiff);

   switch (p(trunk.board[from])) {
      case 0:
         if ((fdabs + rdabs != 1) || !(tmask & JMASK[side]))
            return (move_t){0};
         break;
      case 1: case 3:
         if (fdabs && rdabs)
            return (move_t){0};
         break;
      case 2:
         if (!fdabs || !rdabs || (fdabs + rdabs != 3))
            return (move_t){0};
         break;
      case 4:
         if (fdabs != 1 || rdabs != 1 || !(tmask & SMASK[side]))
            return (move_t){0};
         break;
      case 5:
         if (fdabs != 2 || rdabs != 2 || !(tmask & XMASK[side]))
            return (move_t){0};
         break;
      case 6:
         if ((fdabs + rdabs != 1) || !(tmask & ZMASK[side])
               || (rdiff == (side ? -1 : 1)))
            return (move_t){0};
         break;
   }

   return (move_t){ ._ = {
      from, to, trunk.board[from], trunk.board[to] } };
}

/*!
 * fetch
 * @ fetch board information
 */

void fetch(interface_t* itf) {
   int32_t x; int32_t y;
   getyx(itf->win_state, y, x);
   int64_t index = to_internal(x / 2, RANKS - 1 - y);

   if (itf->index == -1) {
      if (is_index_movable(index)) {
         wchgat(itf->win_state, 1, A_BOLD, 0, NULL);
         wrefresh(itf->win_state);
         itf->index = index;
      }
   } else if (itf->index == index) {
      wchgat(itf->win_state, 1, A_NORMAL, 0, NULL);
      wrefresh(itf->win_state);
      itf->index = -1;
   } else {
      move_t move = move_for_indices(itf->index, index);
      if (move.bits && is_legal(&trunk, move)) {
         advance_history(move);
         advance_game(move);
         wmprint_state(itf);
         refresh_windows(itf, 2, itf->win_fen, itf->win_state);
         itf->index = -1;
      }
   }
}

#define cmds(macro)                       \
   macro(eval), macro(leap),              \
   macro(move), macro(next), macro(quit), \
   macro(redo), macro(undo), macro(zero)

#define list(command)   cmd_##command
#define string(command) #command

enum { cmds(list), ncmds };
static char const* commands[ncmds] = { cmds(string) };

int64_t event_loop(interface_t* itf) {
   for (;;) {
      if (flag(itf, ITF_CURSES)) {
         switch (getch()) {
            case 'e':
               wmprint_info(itf, "%s\n\n", info_eval(itf->info));
               refresh_windows(itf, 1, itf->win_info);
               break;
            case 'f':
               fetch(itf);
               break;
            case 'g':
               if (itf->index != -1)
                  fetch(itf);
               if (itf->index == -1)
                  return 1;
               break;
            case 'h':
               itf->x = itf->x > 1 ? itf->x - 2 : 1;
               refresh_windows(itf, 1, itf->win_state);
               break;
            case 'j':
               itf->y = itf->y < 9 ? itf->y + 1 : 9;
               refresh_windows(itf, 1, itf->win_state);
               break;
            case 'k':
               itf->y = itf->y > 0 ? itf->y - 1 : 0;
               refresh_windows(itf, 1, itf->win_state);
               break;
            case 'l':
               itf->x = itf->x < 17 ? itf->x + 2 : 17;
               refresh_windows(itf, 1, itf->win_state);
               break;
            case 'n':
               itf->index = -1;
               return 1;
            case 'q':
               return 0;
            case 'r':
               redo_history();
               wmprint_state(itf);
               refresh_windows(itf, 2, itf->win_fen, itf->win_state);
               itf->index = -1;
               break;
            case 'u':
               undo_history();
               wmprint_state(itf);
               refresh_windows(itf, 2, itf->win_fen, itf->win_state);
               itf->index = -1;
               break;
            case '~':
               set_state(0);
               wmprint_state(itf);
               refresh_windows(itf, 2, itf->win_fen, itf->win_state);
               itf->index = -1;
               break;
         }
      } else {
         char* buffer = fgets(itf->info->buffer, 128, stdin);
         char** tokens = slice(buffer);

         if (!tokens[0]) {
            free(tokens);
            continue;
         }

         int64_t cmd = -1;
         for (int64_t i = 0; i < ncmds; ++i) {
            if (!strcmp(tokens[0], commands[i])) {
               cmd = i; } }

         int64_t retval = -1;
         switch (cmd) {
            case cmd_eval:
               wmprint(itf, itf->win_info, 0, "%s\n\n", info_eval(itf->info));
               break;
            case cmd_leap:
               retval = 1;
            case cmd_move:
               if (tokens[1] && tokens[2]) {
                  move_t move = move_for_indices(
                     to_internal(atoi(tokens[1]) % FILES,
                                 atoi(tokens[1]) / FILES),
                     to_internal(atoi(tokens[2]) % FILES,
                                 atoi(tokens[2]) / FILES));
                  if (move.bits && is_legal(&trunk, move)) {
                     advance_history(move);
                     advance_game(move);
                     wmprint_state(itf);
                     break;
                  }
               }
               wmprint_info(itf, " - invalid move -\n");
               break;
            case cmd_next:
               retval = 1;
               break;
            case cmd_quit:
               retval = 0;
               break;
            case cmd_redo:
               redo_history();
               wmprint_state(itf);
               break;
            case cmd_undo:
               undo_history();
               wmprint_state(itf);
               break;
            case cmd_zero:
               set_state(tokens[1]);
               wmprint_state(itf);
               break;
            default:
               wmprint_info(itf, " - unknown command: %s\n", tokens[0]);
               break;
         }

         free(tokens);

         if (retval != -1) {
            return retval; }
      }
   }
}
