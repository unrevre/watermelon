#ifndef DEBUG_H
#define DEBUG_H

#include "structs.h"

#include <stdint.h>

extern char cside[2];

/*!
 * game_state
 * @ print game board (prettily)
 */

void game_state();

/*!
 * tree_node_exit
 * @ print node status (on exit) in tree format
 */

void tree_node_exit(uint32_t ply, int32_t alpha, int32_t beta, int32_t score,
                    uint32_t side);

/*!
 * tree_node_entry
 * @ print node status (on entry) in tree format
 */

void tree_node_entry(uint32_t ply, int32_t alpha, int32_t beta, uint32_t side);

/*!
 * move_info
 * @ print move details
 */

void move_info(move_t move, char end);

/*!
 * transposition_table_entry_info
 * @ print transposition table entry details
 */

void transposition_table_entry_info(ttentry_t entry, char end);

#endif /* DEBUG_H */
