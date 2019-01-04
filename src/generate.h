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
 *   including feijiang as literal move
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
 * is_legal
 * @ test if given move is pseudolegal
 */

uint32_t is_legal(move_t move, uint32_t side);

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

#endif /* GENERATE_H */
