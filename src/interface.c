#include "interface.h"

#include "debug.h"
#include "generate.h"
#include "inlines.h"
#include "magics.h"
#include "state.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void init_interface(interface_t* itf, uint64_t mode) {
   itf->info = malloc(sizeof(debug_t));

   init_debug(itf->info);

   itf->mode = mode;
   itf->print = mode ? wmprintw : wmprintf;

   itf->x = 1;
   itf->y = 0;

   itf->index = 0xff;

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

      refresh_windows(itf);
   }
}

void free_interface(interface_t* itf) {
   if (itf->mode) { endwin(); }

   free_debug(itf->info);

   free(itf);
}

void refresh_windows(interface_t* itf) {
   if (itf->mode) {
      wmove(itf->win_state, itf->y, itf->x);

      wnoutrefresh(stdscr);
      wnoutrefresh(itf->border_info);
      wnoutrefresh(itf->win_info);
      wnoutrefresh(itf->border_state);
      wnoutrefresh(itf->win_state);

      doupdate();
   }
}

void refresh_state_window(interface_t* itf) {
   if (itf->mode) {
      wmove(itf->win_state, itf->y, itf->x);

      wnoutrefresh(stdscr);
      wnoutrefresh(itf->win_state);

      doupdate();
   }
}

int wmprintf(WINDOW* w __attribute__((unused)),
             uint64_t clear __attribute__((unused)),
             char const* fmt, va_list args) {
   return vprintf(fmt, args);
}

int wmprintw(WINDOW* w, uint64_t clear, char const* fmt, va_list args) {
   if (clear) { wmove(w, 0, 0); }

   return vwprintw(w, fmt, args);
}

void wmprint(interface_t* itf, WINDOW* w, uint64_t clear, char const* fmt,
             ...) {
   va_list args;
   va_start(args, fmt);
   itf->print(w, clear, fmt, args);
   va_end(args);
}

uint64_t event_loop(interface_t* itf) {
   for (;;) {
      switch (getch()) {
         case 'f':
            fetch(itf);
            break;
         case 'g':
            if (itf->index != 0xff)
               fetch(itf);
            if (itf->index == 0xff)
               return 1;
            break;
         case 'h': case KEY_LEFT:
            itf->x = max(1, itf->x - 2);
            refresh_state_window(itf);
            break;
         case 'j': case KEY_DOWN:
            itf->y = min(9, itf->y + 1);
            refresh_state_window(itf);
            break;
         case 'k': case KEY_UP:
            itf->y = max(0, itf->y - 1);
            refresh_state_window(itf);
            break;
         case 'l': case KEY_RIGHT:
            itf->x = min(17, itf->x + 2);
            refresh_state_window(itf);
            break;
         case 'n':
            itf->index = 0xff;
            return 1;
         case 'q':
            return 0;
         case 'r':
            redo_history();
            wmprint(itf, stdscr, 1, "%s\n", info_fen(itf->info));
            wmprint(itf, itf->win_state, 1, "%s", info_game_state(itf->info));
            refresh_state_window(itf);
            break;
         case 'u':
            undo_history();
            wmprint(itf, stdscr, 1, "%s\n", info_fen(itf->info));
            wmprint(itf, itf->win_state, 1, "%s", info_game_state(itf->info));
            refresh_state_window(itf);
            break;
      }
   }
}

void fetch(interface_t* itf) {
   int32_t x; int32_t y;
   getyx(itf->win_state, y, x);
   uint64_t index = (9 - y) * 9 + x / 2;

   if (itf->index == 0xff) {
      if (is_index_movable(index)) {
         wattron(itf->win_state, A_BOLD);
         waddch(itf->win_state, winch(itf->win_state));
         wattroff(itf->win_state, A_BOLD);
         refresh_state_window(itf);

         itf->index = index;
      }
   } else if (itf->index == index) {
      waddch(itf->win_state, winch(itf->win_state) & A_CHARTEXT);
      refresh_state_window(itf);

      itf->index = 0xff;
   } else {
      move_t move = move_for_indices(itf->index, index);

      if (move.bits && is_legal(move)) {
         advance_with_history(move);

         wmprint(itf, stdscr, 1, "%s\n", info_fen(itf->info));
         wmprint(itf, itf->win_state, 1, "%s", info_game_state(itf->info));
         waddch(itf->win_state, winch(itf->win_state));
         refresh_state_window(itf);

         itf->index = 0xff;
      }
   }
}

void spin(interface_t* itf) {
   if (itf->mode) { getch(); }
}
