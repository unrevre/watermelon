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
 * refresh_all
 * @ refresh all window contents
 */

void refresh_all(interface_t* itf);

/*!
 * refresh_state
 * @ refresh windows displaying game state
 */

void refresh_state(interface_t* itf);

/*!
 * free_interface
 * @ exit ncurses interface and clean up
 */

void free_interface(interface_t* itf);

/*!
 * wmprintw
 * @ wrapper for mvwprintw
 */

int wmprintw(WINDOW* w, uint64_t clear, char const* fmt, va_list args);

/*!
 * wmprintf
 * @ wrapper for printf, discarding extra argument
 */

int wmprintf(WINDOW* w, uint64_t clear, char const* fmt, va_list args);

/*!
 * wmprint
 * @ wrapper for print functions
 */

void wmprint(interface_t* itf, WINDOW* w, uint64_t clear, char const* fmt, ...);

/*!
 * update_state
 * @ helper function - print new game state info and refresh windows
 */

void update_state(interface_t* itf);

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
