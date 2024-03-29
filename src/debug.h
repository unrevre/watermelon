#ifndef DEBUG_H
#define DEBUG_H

#include "magics.h"
#include "structs.h"

#include <stdint.h>

/*!
 * debug_t
 * @ string buffers for informative/debugging purposes
 */

struct debug_t {
   char buffer[256];
   char* buffers[PLYLIMIT];

   char raw[PLYLIMIT][32];
};

/*!
 * init_debug
 * @ initialise buffers
 */

void init_debug(struct debug_t* info);

/*!
 * info_fen
 * @ fill buffer with fen string of current game state
 */

void info_fen(char* buffer);

/*!
 * info_game_state
 * @ fill buffer with current game state
 */

void info_game_state(char* buffer);

/*!
 * info_move
 * @ fill buffer with move details
 */

void info_move(char* buffer, union move_t move);

/*!
 * info_transposition_table_entry
 * @ fill buffer with transposition table entry details
 */

void info_transposition_table_entry(char* buffer, union ttentry_t entry);

/*!
 * info_principal_variation
 * @ trace and fill buffers with principal variation
 */

void info_principal_variation(char** buffers, int64_t depth);

#ifdef INFO
#include "inlines.h"

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

#define debug_printf(...) do { printf(__VA_ARGS__); } while (0)
#else
#define debug_counter_reset(...)
#define debug_counter_increment(...)
#define debug_printf(...)
#endif /* INFO */

#ifdef TREE
#include <stdio.h>

struct transient_t;

/*!
 * tree_debug_state
 * @ callback-like function to redirect transient state variables
 */

void tree_debug_state(struct transient_t* external);

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
