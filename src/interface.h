#ifndef INTERFACE_H
#define INTERFACE_H

#include "debug.h"

#include <ncurses.h>

#include <stdint.h>

/*!
 * interface_t
 * @ interface struct
 */

typedef struct {
   uint64_t mode;
   int (*print) (WINDOW*, uint64_t, char const*, va_list);

   uint32_t x;
   uint32_t y;

   uint64_t index;

   debug_t* info;

   WINDOW* win_state;
   WINDOW* win_info;

   WINDOW* border_state;
   WINDOW* border_info;
} interface_t;

/*!
 * init_interface
 * @ initialise ncurses interface
 */

void init_interface(interface_t* itf, uint64_t mode);

/*!
 * free_interface
 * @ exit ncurses interface and clean up
 */

void free_interface(interface_t* itf);

/*!
 * refresh_all
 * @ refresh all window contents
 */

void refresh_all(interface_t* itf);

/*!
 * refresh_state
 * @ refresh default and game state window contents
 */

void refresh_state(interface_t* itf);

/*!
 * refresh_cursor
 * @ refresh cursor position (refreshes game state window contents)
 */

void refresh_cursor(interface_t* itf);

/*!
 * refresh_info
 * @ refresh info window contents
 */

void refresh_info(interface_t* itf);

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

uint64_t event_loop(interface_t* itf);

/*!
 * fetch
 * @ fetch board information
 */

void fetch(interface_t* itf);

/*!
 * spin
 * @ wait for key press event
 */

void spin(interface_t* itf);

#endif /* INTERFACE_H */
