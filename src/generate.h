#ifndef GENERATE_H
#define GENERATE_H

#include "magics.h"
#include "structs.h"

#include <stdint.h>

extern killer_t ktable[PLYLIMIT];

/*!
 * generate_pseudolegal
 * @ generate all pseudolegal moves
 * # including feijiang as literal move
 */

move_array_t generate_pseudolegal(transient_t* state);

/*!
 * generate_captures
 * @ generate capture moves
 */

move_array_t generate_captures(transient_t* state);

/*!
 * add_piecewise
 * @ add moves (for fixed piece)
 */

void add_piecewise(transient_t* state, __uint128_t set, uint64_t from,
                   move_array_t* moves);

/*!
 * add_shiftwise
 * @ add moves (for fixed direction)
 */

void add_shiftwise(transient_t* state, __uint128_t set, int64_t shift,
                   move_array_t* moves);

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
