#ifndef INTERFACE_H
#define INTERFACE_H

#include <ncurses.h>

#include <stdint.h>

/*!
 * interface_t
 * @ interface struct
 */

typedef struct {
   uint64_t mode;
   int (*print) (WINDOW*, char const*, va_list args);

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
 * init_windows
 * @ initialise windows layout
 */

void init_windows(interface_t* itf);

/*!
 * refresh_windows
 * @ refresh window contents
 */

void refresh_windows(interface_t* itf);

/*!
 * free_interface
 * @ exit ncurses interface and clean up
 */

void free_interface(interface_t* itf);

/*!
 * wmprintf
 * @ wrapper for printf, discarding extra argument
 */

int wmprintf(WINDOW* w, char const* fmt, va_list args);

/*!
 * wmprint
 * @ wrapper for print functions
 */

void wmprint(interface_t* itf, WINDOW* w, char const* fmt, ...);

#endif /* INTERFACE_H */
