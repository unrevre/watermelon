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
      wnoutrefresh(stdscr);
      wnoutrefresh(itf->border_info);
      wnoutrefresh(itf->win_info);
      wnoutrefresh(itf->border_state);
      wnoutrefresh(itf->win_state);

      wmove(itf->win_state, 0, 0);

      doupdate();

      werase(itf->win_state);
   }
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

uint64_t event_loop() {
   for (;;) {
      switch (getch()) {
         case 'n':
            return 1;
         case 'q':
            return 0;
      }
   }
}
