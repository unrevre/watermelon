#ifndef DEBUG_H
#define DEBUG_H

#include "structs.h"

/*!
 * debug_t
 * @ string buffers for informative/debugging purposes
 */

typedef struct {
   char* buffer;
   char** buffers;
} debug_t;

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

char* info_eval(debug_t* info);

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
#include "inlines.h"

#include <stdint.h>
#include <stdio.h>

extern uint64_t nodes;
extern uint64_t qnodes;
extern uint64_t tthits;

/*!
 * debug_counter_reset
 * @ reset counter variables
 */

void debug_counter_reset(void);

/*!
 * debug_counter_increment
 * @ increment counter variable
 */

#define debug_counter_increment(counter) do { \
   atomaddq(&counter, 1); } while (0)

/*!
 * debug_printf
 * @ wrapper for printf with debug target
 */

#define debug_printf(...) printf(__VA_ARGS__)
#else
#define debug_counter_reset(...)
#define debug_counter_increment(...)
#define debug_printf(...)
#endif /* DEBUG */

#ifdef TREE
#include <stdint.h>
#include <stdio.h>

/*!
 * tree_debug_state
 * @ callback-like function to redirect transient state variables
 */

void tree_debug_state(transient_t* external);

/*!
 * tree_root_entry
 * @ print tree root character (entry)
 */

#define tree_root_entry() do { printf("╻\n"); } while (0)

/*!
 * tree_root_exit
 * @ print tree root character (exit)
 */

#define tree_root_exit() do { printf("╹\n"); } while (0)

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

/*!
 * tree_node_message
 * @ print message at node in tree format
 */

void tree_node_message(char const* fmt, ...);
#else
#define tree_debug_state(...)
#define tree_root_entry(...)
#define tree_root_exit(...)
#define tree_node_entry(...)
#define tree_node_exit(...)
#define tree_node_message(...)
#endif /* TREE */

#endif /* DEBUG_H */
