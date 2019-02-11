#ifndef GENERATE_H
#define GENERATE_H

#include "structs.h"

#include <stdint.h>

/*!
 * generate
 * @ generate all moves
 */

move_array_t generate(uint32_t side);

/*!
 * generate_pseudolegal
 * @ generate all pseudolegal moves
 * # including feijiang as literal move
 */

move_array_t generate_pseudolegal(uint32_t side);

/*!
 * generate_captures
 * @ generate capture moves
 */

move_array_t generate_captures(uint32_t side);

/*!
 * generate_quiet
 * @ generate quiet moves
 */

move_array_t generate_quiet(uint32_t side);

/*!
 * in_check
 * @ test if side is in check
 */

uint32_t in_check(uint32_t side);

/*!
 * is_valid
 * @ test if given move is pseudolegal
 */

uint32_t is_valid(move_t move, uint32_t side);

/*!
 * move_for_indices
 * @ return move if indices constitute a valid movement
 */

move_t move_for_indices(uint32_t from, uint32_t to);

/*!
 * add_piecewise
 * @ add moves (for fixed piece)
 */

void add_piecewise(__uint128_t set, uint32_t from, move_array_t* moves);

/*!
 * add_shiftwise
 * @ add moves (for fixed direction)
 */

void add_shiftwise(__uint128_t set, int32_t shift, move_array_t* moves);

/*!
 * sort_moves
 * @ sort generated captures based on mvv/lva
 *
 * # counting sort: group most valuable captures ordered by least valuable
 * attackers. prioritises feijiang moves, but results in suboptimal ordering
 * (compared with traditional mvv - lva) results for some other captures.
 * captures are assumed to be ordered by attacker out of move generation
 */

move_array_t sort_moves(move_array_t moves);

/*!
 * next
 * @ iterate through moves to be searched
 */

move_t next(generator_t* engine);

#endif /* GENERATE_H */
