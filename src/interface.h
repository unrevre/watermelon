#ifndef INTERFACE_H
#define INTERFACE_H

#include "structs.h"

#include <ncurses.h>

#include <stdarg.h>
#include <stdint.h>

#define ITF_CURSES   0x1
#define ITF_QUIET    0x1 << 1

/*!
 * interface_t
 * @ interface struct
 */

typedef struct {
   uint64_t flags;
   int (*print) (WINDOW*, uint64_t, char const*, va_list);
   int32_t x;
   int32_t y;
   int64_t index;

   debug_t* info;

   WINDOW* win_state;
   WINDOW* win_info;
   WINDOW* win_fen;
   WINDOW* border_state;
   WINDOW* border_info;
} interface_t;

#define set_interface(flag, condition) ((condition) ? (flag) : 0)

/*!
 * init_interface
 * @ initialise ncurses interface
 */

void init_interface(interface_t* itf, uint64_t flags);

/*!
 * close_interface
 * @ exit ncurses interface and clean up
 */

void close_interface(interface_t* itf);

/*!
 * refresh_all
 * @ refresh all window contents
 */

void refresh_all(interface_t* itf);

/*!
 * wmprint_state
 * @ helper function - print game state info
 */

void wmprint_state(interface_t* itf);

/*!
 * wmprint_search
 * @ helper function - print search results (move, trace)
 */

void wmprint_search(interface_t* itf, move_t move);

/*!
 * wmprint_info
 * @ helper function - print into info window
 */

void wmprint_info(interface_t* itf, char const* fmt, ...);

/*!
 * event_loop
 * @ event loop handling key events in curses mode
 */

int64_t event_loop(interface_t* itf);

#endif /* INTERFACE_H */
