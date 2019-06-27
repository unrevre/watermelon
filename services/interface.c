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

static union move_t history[STEPLIMIT];

/*!
 * wmprintf
 * @ wrapper for printf, discarding extra argument
 */

static int wmprintf(WINDOW* w __attribute__((unused)), char const* fmt,
                    va_list args) {
   return vprintf(fmt, args);
}

/*!
 * wmprintw
 * @ wrapper for mvwprintw
 */

static int wmprintw(WINDOW* w, char const* fmt, va_list args) {
   return vwprintw(w, fmt, args);
}

/*!
 * wmprint
 * @ wrapper for print functions
 */

static void wmprint(struct interface_t* itf, WINDOW* w, char const* fmt, ...) {
   va_list args;
   va_start(args, fmt);
   itf->print(w, fmt, args);
   va_end(args);
}

/*!
 * update_board
 * @ update game state (fen, board)
 */

static void update_board(struct interface_t* itf) {
   if (flag(itf, ITF_CURSES)) {
      wmove(itf->win_fen, 0, 0);
      wmove(itf->win_state, 0, 0);
   }

   if (flag(itf, ITF_CURSES) || !flag(itf, ITF_QUIET)) {
      info_fen(itf->info->buffer);
      wmprint(itf, itf->win_fen, "%s\n", itf->info->buffer);
      info_game_state(itf->info->buffer);
      wmprint(itf, itf->win_state, "%s", itf->info->buffer);
   }

   if (flag(itf, ITF_CURSES)) {
      wmove(itf->win_state, itf->y, itf->x);
      wnoutrefresh(itf->win_fen);
      wnoutrefresh(itf->win_state);

      doupdate();
   }
}

/*!
 * update_cursor
 * @ helper function to update cursor position
 */

static void update_cursor(struct interface_t* itf) {
   wmove(itf->win_state, itf->y, itf->x);
   wrefresh(itf->win_state);
}

void init_interface(struct interface_t* itf, uint64_t flags) {
   itf->flags = flags;
   itf->print = flag(itf, ITF_CURSES) ? wmprintw : wmprintf;
   itf->x = 1;
   itf->y = 0;
   itf->index = 0;

   itf->info = malloc(sizeof(struct debug_t));
   init_debug(itf->info);

   memset(history, 0, STEPLIMIT * sizeof(union move_t));

   if (flag(itf, ITF_CURSES)) {
      initscr();
      cbreak();
      noecho();

      itf->win_fen = newwin(1, 79, 0, 0);
      itf->win_info = newwin(18, 44, 18, 3);
      itf->win_state = newwin(10, 20, 4, 3);

      scrollok(itf->win_info, TRUE);

      WINDOW* border_info = newwin(20, 48, 17, 1);
      WINDOW* border_state = newwin(14, 23, 2, 1);

      box(border_state, 0, 0);
      box(border_info, 0, 0);

      wnoutrefresh(stdscr);
      wnoutrefresh(border_info);
      wnoutrefresh(border_state);

      doupdate();

      delwin(border_info);
      delwin(border_state);
   }

   update_board(itf);
}

void close_interface(struct interface_t* itf) {
   if (flag(itf, ITF_CURSES)) {
      waddnstr(itf->win_info, "exit\n", 5);
      wrefresh(itf->win_info);

      delwin(itf->win_fen);
      delwin(itf->win_info);
      delwin(itf->win_state);

      getch();
      endwin();
   }

   free(itf->info);
   free(itf);
}

/*!
 * info_search
 * @ update search results (move, trace)
 */

static void info_search(struct interface_t* itf, union move_t move) {
   info_move(itf->info->buffer, move);
   wmprint(itf, itf->win_info, "%s\n\n", itf->info->buffer);

   if (!flag(itf, ITF_QUIET)) {
      info_principal_variation(itf->info->buffers, 0);
      for (char** pv = itf->info->buffers; **pv; ++pv)
         wmprint(itf, itf->win_info, "%s", *pv);
      wmprint(itf, itf->win_info, "\n");
   }

   if (flag(itf, ITF_CURSES))
      wrefresh(itf->win_info);

   fflush(stdout);
}

/*!
 * advance_history
 * @ advance move history records
 */

static void advance_history(union move_t move) {
   int32_t step = trunk.ply;
   union move_t future = history[step];
   if (future.bits && move.bits != future.bits)
      while (history[++step].bits)
         history[step] = (union move_t){0};

   history[trunk.ply] = move;
}

/*!
 * undo_history
 * @ undo last move
 */

static void undo_history(void) {
   if (trunk.ply) { retract_game(history[trunk.ply - 1]); }
}

/*!
 * redo_history
 * @ redo last undone move
 */

static void redo_history(void) {
   if (history[trunk.ply].bits) {
      advance_history(history[trunk.ply]);
      advance_game(history[trunk.ply]);
   }
}

/*!
 * is_index_movable
 * @ test if piece at index is on side to move
 */

static uint32_t is_index_movable(int64_t index) {
   return (trunk.board[index] != empty
      && s(trunk.board[index]) == trunk.side);
}

/*!
 * move_for_indices
 * @ return move if indices constitute a valid move
 */

static union move_t move_for_indices(uint32_t from, uint32_t to) {
   if (is_index_movable(to)) { return (union move_t){0}; }

   int64_t side = trunk.side;
   __uint128_t tmask = PMASK[to];

   int32_t fdiff = (from - OFFSET) % WIDTH - (to - OFFSET) % WIDTH;
   int32_t fdabs = abs(fdiff);
   int32_t rdiff = (from - OFFSET) / WIDTH - (to - OFFSET) / WIDTH;
   int32_t rdabs = abs(rdiff);

   switch (p(trunk.board[from])) {
      case 0:
         if ((fdabs + rdabs != 1) || !(tmask & JMASK[side]))
            return (union move_t){0};
         break;
      case 1: case 3:
         if (fdabs && rdabs)
            return (union move_t){0};
         break;
      case 2:
         if (!fdabs || !rdabs || (fdabs + rdabs != 3))
            return (union move_t){0};
         break;
      case 4:
         if (fdabs != 1 || rdabs != 1 || !(tmask & SMASK[side]))
            return (union move_t){0};
         break;
      case 5:
         if (fdabs != 2 || rdabs != 2 || !(tmask & XMASK[side]))
            return (union move_t){0};
         break;
      case 6:
         if ((fdabs + rdabs != 1) || !(tmask & ZMASK[side])
               || (rdiff == (side ? -1 : 1)))
            return (union move_t){0};
         break;
   }

   return (union move_t){ ._ = {
      from, to, trunk.board[from], trunk.board[to] } };
}

/*!
 * fetch
 * @ fetch board information
 */

static void fetch(struct interface_t* itf) {
   int32_t x; int32_t y;
   getyx(itf->win_state, y, x);
   int64_t index = index_for(x / 2, 9 - y);

   if (itf->index == 0 && is_index_movable(index)) {
      wchgat(itf->win_state, 1, A_BOLD, 0, NULL);
      wrefresh(itf->win_state);
      itf->index = index;
   } else {
      if (itf->index != index) {
         union move_t move = move_for_indices(itf->index, index);
         if (!move.bits || !is_legal(&trunk, move)) { return; }
         advance_history(move);
         advance_game(move);
      }

      update_board(itf);
      itf->index = 0;
   }
}

#define cmds(macro)                       \
   macro(move), macro(next), macro(quit), \
   macro(redo), macro(undo)

#define list(command)   cmd_##command
#define string(command) #command

enum { cmds(list), ncmds };
static char const* commands[ncmds] = { cmds(string) };

int64_t event_loop(struct interface_t* itf) {
   for (;;) {
      if (flag(itf, ITF_CURSES)) {
         switch (getch()) {
            case 'f':
               fetch(itf);
               break;
            case 'g':
               if (itf->index != 0)
                  fetch(itf);
               return 1;
            case 'h':
               itf->x = itf->x > 1 ? itf->x - 2 : 1;
               update_cursor(itf);
               break;
            case 'j':
               itf->y = itf->y < 9 ? itf->y + 1 : 9;
               update_cursor(itf);
               break;
            case 'k':
               itf->y = itf->y > 0 ? itf->y - 1 : 0;
               update_cursor(itf);
               break;
            case 'l':
               itf->x = itf->x < 17 ? itf->x + 2 : 17;
               update_cursor(itf);
               break;
            case 'n':
               itf->index = 0;
               return 1;
            case 'q':
               return 0;
            case 'r':
               redo_history();
               update_board(itf);
               itf->index = 0;
               break;
            case 'u':
               undo_history();
               update_board(itf);
               itf->index = 0;
               break;
         }
      } else {
         char* buffer = fgets(itf->info->buffer, 128, stdin);
         char** tokens = slice(itf->info->buffers, buffer);

         if (!tokens[0]) { continue; }

         int64_t cmd;
         for (cmd = 0; cmd < ncmds; ++cmd) {
            if (!strcmp(tokens[0], commands[cmd])) {
               break; } }

         switch (cmd) {
            case cmd_move:
               if (tokens[1] && tokens[2]) {
                  union move_t move = move_for_indices(
                     to_internal(atoi(tokens[1])),
                     to_internal(atoi(tokens[2])));
                  if (move.bits && is_legal(&trunk, move)) {
                     advance_history(move);
                     advance_game(move);
                     update_board(itf);
                  } else {
                     wmprint(itf, itf->win_info,  "invalid move\n");
                  }
               }
               break;
            case cmd_next:
               return 1;
            case cmd_quit:
               return 0;
            case cmd_redo:
               redo_history();
               update_board(itf);
               break;
            case cmd_undo:
               undo_history();
               update_board(itf);
               break;
            default:
               wmprint(itf, itf->win_info, "unknown command\n");
               break;
         }
      }
   }
}

int64_t update(struct interface_t* itf, union move_t move) {
   return is_legal(&trunk, move) && (advance_history(move), advance_game(move),
      info_search(itf, move), update_board(itf), 1);
}
