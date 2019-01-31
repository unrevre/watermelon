#ifndef SEARCH_H
#define SEARCH_H

#include "structs.h"

#include <stdint.h>

/*!
 * iter_dfs
 * @ iterative deepening
 */

move_t iter_dfs(uint32_t depth, uint32_t side);

/*!
 * negamax
 * @ negamax search, with alpha-beta pruning (fail-soft)
 */

int32_t negamax(uint32_t depth, int32_t alpha, int32_t beta, uint32_t side);

/*!
 * quiescence
 * @ quiescence search (fail-soft)
 */

int32_t quiescence(int32_t alpha, int32_t beta, uint32_t side);

/*!
 * store_hash
 * @ transposition table replacement scheme
 */

void store_hash(uint32_t depth, int32_t alpha, int32_t beta, int32_t score,
                move_t move_hash);

#endif /* SEARCH_H */
