#ifndef DEBUG_H
#define DEBUG_H

#include "structs.h"

#include <stdint.h>

/*!
 * init_debug
 * @ initialise buffers
 */

void init_debug(debug_t* info);

/*!
 * free_debug
 * @ free allocated memory
 */

void free_debug(debug_t* info);

/*!
 * info_eval
 * @ return internal evaluation of current game state
 */

char* info_eval(debug_t* info, int64_t side);

/*!
 * resp_eval
 * @ return internal evaluation of current game state, formatted as response
 */

char* resp_eval(debug_t* info);

/*!
 * info_fen
 * @ return fen string of current game state
 */

char* info_fen(debug_t* info);

/*!
 * info_game_state
 * @ return string of current game state
 */

char* info_game_state(debug_t* info);

/*!
 * info_move
 * @ return string of move details
 */

char* info_move(debug_t* info, move_t move);

/*!
 * resp_move
 * @ return string of move details, formatted as response
 */

char* resp_move(debug_t* info, move_t move);

/*!
 * info_transposition_table_entry
 * @ return string of transposition table entry details
 */

char* info_transposition_table_entry(debug_t* info, ttentry_t entry);

/*!
 * info_principal_variation
 * @ trace and return array of strings of principal variation
 */

char** info_principal_variation(debug_t* info);

#ifdef DEBUG
extern uint64_t nodes;
extern uint64_t qnodes;

extern uint64_t tthits;

/*!
 * debug_variable_reset
 * @ reset debug variable(s)
 */

void debug_variable_reset(int64_t count, ...);

/*!
 * debug_variable_increment
 * @ increment debug variable(s)
 */

void debug_variable_increment(int64_t count, ...);

/*!
 * debug_variable_headers
 * @ print labels for debug variable(s)
 */

void debug_variable_headers(int64_t count, ...);

/*!
 * debug_variable_values
 * @ print value of debug variable(s)
 */

void debug_variable_values(int64_t count, ...);

/*!
 * debug_printf
 * @ wrapper for printf with debug target
 */

void debug_printf(char const* fmt, ...);
#else
#define debug_variable_reset(count, ...)
#define debug_variable_increment(count, ...)
#define debug_variable_headers(count, ...)
#define debug_variable_values(count, ...)
#define debug_printf(fmt, ...)
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
