#ifndef GENERATE_H
#define GENERATE_H

#include "structs.h"

#include <stdint.h>

/*!
 * generate
 * @ generate all moves
 */

move_array_t generate(int64_t side);

/*!
 * generate_pseudolegal
 * @ generate all pseudolegal moves
 * # including feijiang as literal move
 */

move_array_t generate_pseudolegal(int64_t side);

/*!
 * generate_captures
 * @ generate capture moves
 */

move_array_t generate_captures(int64_t side);

/*!
 * generate_quiet
 * @ generate quiet moves
 */

move_array_t generate_quiet(int64_t side);

/*!
 * add_piecewise
 * @ add moves (for fixed piece)
 */

void add_piecewise(__uint128_t set, uint64_t from, move_array_t* moves);

/*!
 * add_shiftwise
 * @ add moves (for fixed direction)
 */

void add_shiftwise(__uint128_t set, int64_t shift, move_array_t* moves);

/*!
 * sort_moves
 * @ sort generated captures based on mvv/lva
 *
 * # counting sort: group most valuable captures ordered by least valuable
 * attackers. prioritises feijiang moves, but results in suboptimal ordering
 * (compared with traditional mvv - lva) results for some other captures.
 * captures are assumed to be ordered by attacker out of move generation
 */

void sort_moves(move_array_t* moves);

/*!
 * next
 * @ iterate through moves to be searched
 */

move_t next(generator_t* engine, transient_t* state);

#endif /* GENERATE_H */
