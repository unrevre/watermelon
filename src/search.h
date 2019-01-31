#ifndef SEARCH_H
#define SEARCH_H

#include "structs.h"

#include <stdint.h>

/*!
 * iter_dfs
 * @ iterative deepening
 */

move_t iter_dfs(uint32_t depth);

/*!
 * negamax
 * @ negamax search, with alpha-beta pruning (fail-soft)
 */

int32_t negamax(uint32_t depth, int32_t alpha, int32_t beta);

/*!
 * quiescence
 * @ quiescence search (fail-soft)
 */

int32_t quiescence(int32_t alpha, int32_t beta);

#endif /* SEARCH_H */
