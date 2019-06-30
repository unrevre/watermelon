#ifndef INTERFACE_H
#define INTERFACE_H

#include <curses.h>

#include <stdarg.h>
#include <stdint.h>

union move_t;

#define ITF_CURSES   0x1
#define ITF_QUIET    0x1 << 1

#define set(flag, boolean) ((boolean) ? (flag) : 0)

/*!
 * interface_t
 * @ interface struct
 */

struct interface_t {
   uint64_t flags;
   int (*print) (WINDOW*, char const*, va_list);
   int32_t x;
   int32_t y;
   int64_t index;

   struct debug_t* info;

   WINDOW* win_state;
   WINDOW* win_info;
   WINDOW* win_fen;
};

/*!
 * init_interface
 * @ initialise ncurses interface
 */

void init_interface(struct interface_t* itf, uint64_t flags);

/*!
 * close_interface
 * @ exit ncurses interface and clean up
 */

void close_interface(struct interface_t* itf);

/*!
 * event_loop
 * @ event loop handling key events in curses mode
 */

int64_t event_loop(struct interface_t* itf);

/*!
 * update
 * @ update interface conditionally
 */

int64_t update(struct interface_t* itf, union move_t move);

#endif /* INTERFACE_H */
