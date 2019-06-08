#ifndef GENERATE_H
#define GENERATE_H

#include "structs.h"

#include <stdint.h>

/*!
 * generate_pseudolegal
 * @ generate all pseudolegal moves
 * # including feijiang as literal move
 */

struct move_array_t generate_pseudolegal(struct transient_t* state);

/*!
 * generate_captures
 * @ generate capture moves
 */

struct move_array_t generate_captures(struct transient_t* state);

/*!
 * add_piecewise
 * @ add moves (for fixed piece)
 */

void add_piecewise(struct transient_t* state, __uint128_t set, uint64_t from,
                   struct move_array_t* moves);

/*!
 * add_shiftwise
 * @ add moves (for fixed direction)
 */

void add_shiftwise(struct transient_t* state, __uint128_t set, int64_t shift,
                   struct move_array_t* moves);

/*!
 * sort_moves
 * @ sort generated captures based on mvv/lva
 *
 * # counting sort: group most valuable captures ordered by least valuable
 * attackers. prioritises feijiang moves, but results in suboptimal ordering
 * (compared with traditional mvv - lva) results for some other captures.
 * captures are assumed to be ordered by attacker out of move generation
 */

void sort_moves(struct move_array_t* moves);

#endif /* GENERATE_H */
