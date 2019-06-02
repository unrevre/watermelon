#ifndef SEARCH_H
#define SEARCH_H

#include "structs.h"

#include <stdint.h>

/*!
 * killer_t
 * @ killer move struct
 */

typedef struct {
   move_t first;
   move_t second;
   int64_t count;
} killer_t;

/*!
 * generator_t
 * @ move ordering controller
 */

typedef struct {
   int64_t state;
   int64_t index;
   move_array_t moves;
   move_t move;
} generator_t;

extern killer_t ktable[PLYLIMIT];

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

/*!
 * next
 * @ iterate through moves to be searched
 */

move_t next(generator_t* engine, transient_t* state);

#endif /* SEARCH_H */
