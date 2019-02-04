#ifndef DEBUG_H
#define DEBUG_H

#include "structs.h"

#include <stdint.h>

/*!
 * info_game_state
 * @ print current game state
 */

int32_t info_game_state(char* buffer);

/*!
 * info_move
 * @ print move details
 */

int32_t info_move(char* buffer, move_t move);

/*!
 * info_transposition_table_entry
 * @ print transposition table entry details
 */

int32_t info_transposition_table_entry(char* buffer, ttentry_t entry);

#ifdef DEBUG
extern uint32_t nodes;
extern uint32_t qnodes;

extern uint32_t tthits;

/*!
 * debug_variable_reset
 * @ reset debug variable(s)
 */

void debug_variable_reset(uint32_t count, ...);

/*!
 * debug_variable_increment
 * @ increment debug variable(s)
 */

void debug_variable_increment(uint32_t count, ...);

/*!
 * debug_variable_headers
 * @ print labels for debug variable(s)
 */

void debug_variable_headers(uint32_t count, ...);

/*!
 * debug_variable_values
 * @ print value of debug variable(s)
 */

void debug_variable_values(uint32_t count, ...);
#else
#define debug_variable_reset(count, ...)
#define debug_variable_increment(count, ...)
#define debug_variable_headers(count, ...)
#define debug_variable_values(count, ...)
#endif /* DEBUG */

#ifdef TREE
/*!
 * tree_root_entry
 * @ print tree root character (entry)
 */

void tree_root_entry(void);

/*!
 * tree_root_exit
 * @ print tree root character (exit)
 */

void tree_root_exit(void);

/*!
 * tree_node_entry
 * @ print node status (on entry) in tree format
 */

void tree_node_entry(int32_t alpha, int32_t beta);

/*!
 * tree_node_exit
 * @ print node status (on exit) in tree format
 */

void tree_node_exit(int32_t alpha, int32_t beta, int32_t score);
#else
#define tree_root_entry(...)
#define tree_root_exit(...)
#define tree_node_entry(...)
#define tree_node_exit(...)
#endif /* TREE */

#endif /* DEBUG_H */
