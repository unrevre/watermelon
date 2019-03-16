#include "interface.h"

#include "debug.h"
#include "generate.h"
#include "memory.h"
#include "position.h"
#include "state.h"
#include "structs.h"
#include "utilities.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void init_interface(interface_t* itf, int64_t mode, int64_t quiet) {
   itf->mode = mode;
   itf->print = mode ? wmprintw : wmprintf;
   itf->x = 1;
   itf->y = 0;
   itf->index = -1;
   itf->quiet = quiet;

   itf->info = malloc(sizeof(debug_t));
   init_debug(itf->info);

   if (itf->mode) {
      initscr();
      cbreak();
      noecho();

      itf->border_info = newwin(20, 48, 17, 1);
      itf->win_info = newwin(18, 44, 18, 3);
      itf->border_state = newwin(14, 23, 2, 1);
      itf->win_state = newwin(10, 20, 4, 3);

      keypad(stdscr, TRUE);
      scrollok(itf->win_info, TRUE);
      box(itf->border_state, 0, 0);
      box(itf->border_info, 0, 0);

      wnoutrefresh(stdscr);
      wnoutrefresh(itf->border_info);
      wnoutrefresh(itf->border_state);

      doupdate();
   }
}

void close_interface(interface_t* itf) {
   wmprint_info(itf, " - exit -\n");
   refresh_info(itf);

   if (itf->mode) {
      getch();
      endwin();
   }

   free_debug(itf->info);
   free(itf);
}

void refresh_all(interface_t* itf) {
   if (itf->mode) {
      wmove(itf->win_state, itf->y, itf->x);

      wnoutrefresh(stdscr);
      wnoutrefresh(itf->win_info);
      wnoutrefresh(itf->win_state);

      doupdate();
   }
}

void refresh_state(interface_t* itf) {
   if (itf->mode) {
      wmove(itf->win_state, itf->y, itf->x);

      wnoutrefresh(stdscr);
      wnoutrefresh(itf->win_state);

      doupdate();
   }
}

void refresh_cursor(interface_t* itf) {
   if (itf->mode) {
      wmove(itf->win_state, itf->y, itf->x);

      wrefresh(itf->win_state);
   }
}

void refresh_info(interface_t* itf) {
   if (itf->mode) { wrefresh(itf->win_info); }
}

void wmprint_state(interface_t* itf) {
   if (itf->quiet) { return; }

   wmprint(itf, stdscr, 1, "%s\n", info_fen(itf->info));
   wmprint(itf, itf->win_state, 1, "%s", info_game_state(itf->info));
}

void wmprint_search(interface_t* itf, move_t move) {
   wmprint(itf, itf->win_info, 0, "%s\n\n", info_move(itf->info, move));
   if (itf->quiet) { return; }

   wmprint(itf, itf->win_info, 0, "%s\n", info_principal_variation(itf->info));
}

void wmprint_info(interface_t* itf, char const* fmt, ...) {
   if (itf->quiet) { return; }

   va_list args;
   va_start(args, fmt);
   itf->print(itf->win_info, 0, fmt, args);
   va_end(args);
}

void update_state(interface_t* itf) {
   wmprint_state(itf);
   refresh_state(itf);
}

int64_t event_loop(interface_t* itf) {
   for (;;) {
      if (itf->mode) {
         switch (getch()) {
            case 'f':
               fetch(itf);
               break;
            case 'g':
               if (itf->index != -1)
                  fetch(itf);
               if (itf->index == -1)
                  return 1;
               break;
            case 'h': case KEY_LEFT:
               itf->x = itf->x > 1 ? itf->x - 2 : 1;
               refresh_cursor(itf);
               break;
            case 'j': case KEY_DOWN:
               itf->y = itf->y < 9 ? itf->y + 1 : 9;
               refresh_cursor(itf);
               break;
            case 'k': case KEY_UP:
               itf->y = itf->y > 0 ? itf->y - 1 : 0;
               refresh_cursor(itf);
               break;
            case 'l': case KEY_RIGHT:
               itf->x = itf->x < 17 ? itf->x + 2 : 17;
               refresh_cursor(itf);
               break;
            case 'n':
               itf->index = -1;
               return 1;
            case 'q':
               return 0;
            case 'r':
               redo_history();
               update_state(itf);
               itf->index = -1;
               break;
            case 'u':
               undo_history();
               update_state(itf);
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

         if (!strcmp(tokens[0], "move")) {
            int32_t from = atoi(tokens[1]);
            int32_t to = atoi(tokens[2]);

            if (from < 0 || to < 0 || from > 89 || to > 89) {
               wmprint_info(itf, " - invalid indices -\n");
            } else {
               move_t move = move_for_indices(from, to);
               if (move.bits && is_legal(move)) {
                  advance_history(move);
                  advance_game(move);
                  update_state(itf);
               } else {
                  wmprint_info(itf, " - invalid move -\n");
               }
            }
         } else if (!strcmp(tokens[0], "next")) {
            free(tokens);
            return 1;
         } else if (!strcmp(tokens[0], "quit")) {
            free(tokens);
            return 0;
         } else if (!strcmp(tokens[0], "undo")) {
            undo_history();
            update_state(itf);
         } else if (!strcmp(tokens[0], "redo")) {
            redo_history();
            update_state(itf);
         } else if (!strcmp(tokens[0], "reset")) {
            reset_state(tokens[1]);
            update_state(itf);
         } else {
            wmprint_info(itf, " - unknown command: %s\n", tokens[0]);
         }

         free(tokens);
      }
   }
}

void fetch(interface_t* itf) {
   int32_t x; int32_t y;
   getyx(itf->win_state, y, x);
   int64_t index = (9 - y) * 9 + x / 2;

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
      if (move.bits && is_legal(move)) {
         advance_history(move);
         advance_game(move);
         update_state(itf);
         itf->index = -1;
      }
   }
}
