#ifndef POSITION_H
#define POSITION_H

#include "structs.h"

#include <stdint.h>

/*!
 * in_check
 * @ test if side is in check
 */

uint32_t in_check(transient_t* state, int64_t side);

/*!
 * is_valid
 * @ test if given move is pseudolegal
 */

uint32_t is_valid(transient_t* state, move_t move);

/*!
 * is_legal
 * @ test if given move is legal in current position
 */

uint32_t is_legal(transient_t* state, move_t move);

/*!
 * is_repetition
 * @ test if current position is in state of repetition
 */

uint32_t is_repetition(transient_t* state);

/*!
 * is_index_movable
 * @ test if piece at index is on side to move
 */

uint32_t is_index_movable(int64_t index);

/*!
 * move_for_indices
 * @ return move if indices constitute a valid movement
 */

move_t move_for_indices(uint32_t from, uint32_t to);

#endif /* POSITION_H */
