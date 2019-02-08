#include "interface.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void init_interface(interface_t* itf, uint64_t mode) {
   itf->mode = mode;
   itf->print = mode ? vwprintw : wmprintf;

   if (itf->mode) {
      initscr();
      cbreak();
      noecho();
   }

   init_windows(itf);
   refresh_windows(itf);
}

void init_windows(interface_t* itf) {
   if (itf->mode) {
      itf->win_state = newwin(10, 20, 4, 3);
      itf->win_info = newwin(12, 44, 18, 3);

      itf->border_state = newwin(14, 23, 2, 1);
      itf->border_info = newwin(16, 48, 17, 1);

      box(itf->border_state, 0, 0);
      box(itf->border_info, 0, 0);

      wmove(itf->win_state, 0, 0);

      keypad(stdscr, TRUE);
   }
}

void refresh_windows(interface_t* itf) {
   if (itf->mode) {
      wrefresh(stdscr);

      wrefresh(itf->win_state);
      wrefresh(itf->win_info);

      wrefresh(itf->border_state);
      wrefresh(itf->border_info);
   }
}

void free_interface(interface_t* itf) {
   if (itf->mode) { endwin(); }

   free(itf);
}

int wmprintf(WINDOW* w __attribute__((unused)), char const* fmt, va_list args) {
   return vprintf(fmt, args);
}

void wmprint(interface_t* itf, WINDOW* w, char const* fmt, ...) {
   va_list args;
   va_start(args, fmt);
   itf->print(w, fmt, args);
   va_end(args);
}
