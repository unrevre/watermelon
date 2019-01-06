#ifndef DEBUG_H
#define DEBUG_H

#include <stdint.h>

/*!
 * info_game_state
 * @ print current game state
 */

void info_game_state(void);

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

#endif /* DEBUG_H */
