#include "interface.h"

#include "inlines.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void init_interface(interface_t* itf, uint64_t mode) {
   itf->mode = mode;
   itf->print = mode ? wmprintw : wmprintf;

   itf->x = 1;
   itf->y = 0;

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

void refresh_state(interface_t* itf) {
   if (itf->mode) {
      wmove(itf->win_state, itf->y, itf->x);

      wrefresh(itf->win_state);
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
         case 'h': case KEY_LEFT:
            itf->x = max(1, itf->x - 2);
            refresh_state(itf);
            break;
         case 'j': case KEY_DOWN:
            itf->y = min(9, itf->y + 1);
            refresh_state(itf);
            break;
         case 'k': case KEY_UP:
            itf->y = max(0, itf->y - 1);
            refresh_state(itf);
            break;
         case 'l': case KEY_RIGHT:
            itf->x = min(17, itf->x + 2);
            refresh_state(itf);
            break;
         case 'n':
            return 1;
         case 'q':
            return 0;
      }
   }
}
