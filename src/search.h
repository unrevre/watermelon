#ifndef SEARCH_H
#define SEARCH_H

#include "structs.h"

/*!
 * iter_dfs
 * @ iterative deepening
 */

move_t iter_dfs(uint32_t depth, uint32_t side);

/*!
 * negamax
 * @ negamax search, with alpha-beta pruning (fail-soft)
 */

int32_t negamax(uint32_t depth, uint32_t ply, int32_t alpha, int32_t beta,
                uint32_t side);

#endif /* SEARCH_H */
