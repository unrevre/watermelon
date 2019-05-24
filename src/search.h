#ifndef SEARCH_H
#define SEARCH_H

#include "structs.h"

#include <stdint.h>

/*!
 * iter_dfs
 * @ iterative deepening
 */

void iter_dfs(transient_t* state);

/*!
 * negamax
 * @ negamax search, with alpha-beta pruning (fail-soft)
 */

int32_t negamax(int32_t depth, transient_t* state, int32_t alpha, int32_t beta,
                uint32_t principal);

/*!
 * quiescence
 * @ quiescence search (fail-soft)
 */

int32_t quiescence(transient_t* state, int32_t alpha, int32_t beta);

#endif /* SEARCH_H */
