#include "interface.h"

#include "magics.h"
#include "masks.h"
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
             char const* fmt, va_list args) {
   return vprintf(fmt, args);
}

/*!
 * wmprintw
 * @ wrapper for mvwprintw
 */

int wmprintw(WINDOW* w, char const* fmt, va_list args) {
   return vwprintw(w, fmt, args);
}

/*!
 * wmprint
 * @ wrapper for print functions
 */

void wmprint(struct interface_t* itf, WINDOW* w, char const* fmt, ...) {
   va_list args;
   va_start(args, fmt);
   itf->print(w, fmt, args);
   va_end(args);
}

void init_interface(struct interface_t* itf, uint64_t flags) {
   itf->flags = flags;
   itf->print = flag(itf, ITF_CURSES) ? wmprintw : wmprintf;
   itf->x = 1;
   itf->y = 0;
   itf->index = 0;

   itf->info = malloc(sizeof(struct debug_t));
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

void close_interface(struct interface_t* itf) {
   wmprint_info(itf, " - exit -\n");

   if (flag(itf, ITF_CURSES)) {
      wrefresh(itf->win_info);
      getch();
      endwin();
   }

   free_debug(itf->info);
   free(itf);
}

void refresh_interface(struct interface_t* itf) {
   if (flag(itf, ITF_CURSES)) {
      wmove(itf->win_state, itf->y, itf->x);
      wnoutrefresh(itf->win_fen);
      wnoutrefresh(itf->win_info);
      wnoutrefresh(itf->win_state);

      doupdate();
   }
}

void refresh_board(struct interface_t* itf) {
   if (flag(itf, ITF_CURSES)) {
      wmove(itf->win_state, itf->y, itf->x);
      wrefresh(itf->win_state);
   }
}

void refresh_state(struct interface_t* itf) {
   if (flag(itf, ITF_CURSES)) {
      wmove(itf->win_state, itf->y, itf->x);
      wnoutrefresh(itf->win_fen);
      wnoutrefresh(itf->win_state);

      doupdate();
   }
}

void wmprint_state(struct interface_t* itf) {
   if (flag(itf, ITF_CURSES)) {
      wmove(itf->win_fen, 0, 0);
      wmove(itf->win_state, 0, 0);
   }

   if (flag(itf, ITF_CURSES) || !flag(itf, ITF_QUIET)) {
      wmprint(itf, itf->win_fen, "%s\n", info_fen(itf->info));
      wmprint(itf, itf->win_state, "%s", info_game_state(itf->info));
   }
}

void wmprint_search(struct interface_t* itf, move_t move) {
   wmprint(itf, itf->win_info, "%s\n", info_move(itf->info, move));
   if (!flag(itf, ITF_QUIET)) {
      wmprint_info(itf, "\n");
      for (char** pv = info_principal_variation(itf->info); **pv; ++pv)
         wmprint_info(itf, "%s", *pv);
      wmprint_info(itf, "\n");
   }

   fflush(stdout);
}

void wmprint_info(struct interface_t* itf, char const* fmt, ...) {
   if (!flag(itf, ITF_QUIET)) {
      va_list args;
      va_start(args, fmt);
      itf->print(itf->win_info, fmt, args);
      va_end(args);
   }
}

int64_t advance_if_legal(move_t move) {
   return is_legal(&trunk, move)
      && (advance_history(move), advance_game(move), 1);
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

void fetch(struct interface_t* itf) {
   int32_t x; int32_t y;
   getyx(itf->win_state, y, x);
   int64_t index = index_for(x / 2, RANKS - 1 - y);

   if (itf->index == 0) {
      if (is_index_movable(index)) {
         wchgat(itf->win_state, 1, A_BOLD, 0, NULL);
         wrefresh(itf->win_state);
         itf->index = index;
      }
   } else if (itf->index == index) {
      wchgat(itf->win_state, 1, A_NORMAL, 0, NULL);
      wrefresh(itf->win_state);
      itf->index = 0;
   } else {
      move_t move = move_for_indices(itf->index, index);
      if (move.bits && advance_if_legal(move)) {
         wmprint_state(itf);
         refresh_state(itf);
         itf->index = 0;
      }
   }
}

#define cmds(macro)                       \
   macro(eval),                           \
   macro(move), macro(next), macro(quit), \
   macro(redo), macro(undo), macro(zero)

#define list(command)   cmd_##command
#define string(command) #command

enum { cmds(list), ncmds };
static char const* commands[ncmds] = { cmds(string) };

int64_t event_loop(struct interface_t* itf) {
   for (;;) {
      if (flag(itf, ITF_CURSES)) {
         switch (getch()) {
            case 'e':
               wmprint_info(itf, "%s\n\n", info_eval(itf->info));
               if (flag(itf, ITF_CURSES)) {
                  wrefresh(itf->win_info); }
               break;
            case 'f':
               fetch(itf);
               break;
            case 'g':
               if (itf->index != 0)
                  fetch(itf);
               if (itf->index == 0)
                  return 1;
               break;
            case 'h':
               itf->x = itf->x > 1 ? itf->x - 2 : 1;
               refresh_board(itf);
               break;
            case 'j':
               itf->y = itf->y < 9 ? itf->y + 1 : 9;
               refresh_board(itf);
               break;
            case 'k':
               itf->y = itf->y > 0 ? itf->y - 1 : 0;
               refresh_board(itf);
               break;
            case 'l':
               itf->x = itf->x < 17 ? itf->x + 2 : 17;
               refresh_board(itf);
               break;
            case 'n':
               itf->index = 0;
               return 1;
            case 'q':
               return 0;
            case 'r':
               redo_history();
               wmprint_state(itf);
               refresh_state(itf);
               itf->index = 0;
               break;
            case 'u':
               undo_history();
               wmprint_state(itf);
               refresh_state(itf);
               itf->index = 0;
               break;
            case '~':
               set_state(0);
               wmprint_state(itf);
               refresh_state(itf);
               itf->index = 0;
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
               wmprint(itf, itf->win_info, "%s\n\n", info_eval(itf->info));
               break;
            case cmd_next:
               retval = 1;
            case cmd_move:
               if (tokens[1] && tokens[2]) {
                  move_t move = move_for_indices(to_internal(atoi(tokens[1])),
                                                 to_internal(atoi(tokens[2])));
                  if (move.bits && advance_if_legal(move)) {
                     wmprint_state(itf);
                  } else {
                     wmprint_info(itf, " - invalid move -\n");
                  }
               }
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
