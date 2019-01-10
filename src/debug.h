#ifndef DEBUG_H
#define DEBUG_H

#include "structs.h"

#include <stdint.h>

/*!
 * info_game_state
 * @ print current game state
 */

void info_game_state(void);

/*!
 * info_move
 * @ print move details
 */

void info_move(move_t move, char end);

/*!
 * info_transposition_table_entry
 * @ print transposition table entry details
 */

void info_transposition_table_entry(ttentry_t entry, char end);

/*!
 * debug_node_counts
 * @ print visited node counts in search
 */

void debug_node_counts(uint32_t nodes, uint32_t qnodes, uint32_t tthits);

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

void tree_node_entry(uint32_t ply, int32_t alpha, int32_t beta, uint32_t side);

/*!
 * tree_node_exit
 * @ print node status (on exit) in tree format
 */

void tree_node_exit(uint32_t ply, int32_t alpha, int32_t beta, int32_t score,
                    uint32_t side);

#endif /* DEBUG_H */
