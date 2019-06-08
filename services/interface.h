#ifndef INTERFACE_H
#define INTERFACE_H

#include "debug.h"
#include "structs.h"

#include <ncurses.h>

#include <stdarg.h>
#include <stdint.h>

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
   WINDOW* border_state;
   WINDOW* border_info;
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
 * refresh_interface
 * @ refresh all window contents
 */

void refresh_interface(struct interface_t* itf);

/*!
 * wmprint_state
 * @ helper function - print game state info
 */

void wmprint_state(struct interface_t* itf);

/*!
 * wmprint_search
 * @ helper function - print search results (move, trace)
 */

void wmprint_search(struct interface_t* itf, union move_t move);

/*!
 * wmprint_info
 * @ helper function - print into info window
 */

void wmprint_info(struct interface_t* itf, char const* fmt, ...);

/*!
 * advance_if_legal
 * @ helper function - test legality and advance game, history
 */

int64_t advance_if_legal(union move_t move);

/*!
 * event_loop
 * @ event loop handling key events in curses mode
 */

int64_t event_loop(struct interface_t* itf);

#endif /* INTERFACE_H */
