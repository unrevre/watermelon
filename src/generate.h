#ifndef GENERATE_H
#define GENERATE_H

#include "structs.h"

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
