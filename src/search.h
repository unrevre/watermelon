#ifndef SEARCH_H
#define SEARCH_H

#include "magics.h"
#include "structs.h"

#include <stdint.h>

/*!
 * killer_t
 * @ killer move struct
 */

struct killer_t {
   union move_t first;
   union move_t second;
   int64_t count;
};

/*!
 * generator_t
 * @ move ordering controller
 */

struct generator_t {
   int64_t state;
   int64_t index;
   struct move_array_t moves;
   union move_t move;
};

extern struct killer_t ktable[PLYLIMIT];

/*!
 * iter_dfs
 * @ iterative deepening
 */

void iter_dfs(struct transient_t* state);

/*!
 * negamax
 * @ negamax search, with alpha-beta pruning (fail-soft)
 */

int32_t negamax(int32_t depth, struct transient_t* state, int32_t alpha,
                int32_t beta, uint32_t principal);

/*!
 * quiescence
 * @ quiescence search (fail-soft)
 */

int32_t quiescence(struct transient_t* state, int32_t alpha, int32_t beta);

/*!
 * next
 * @ iterate through moves to be searched
 */

union move_t next(struct generator_t* engine, struct transient_t* state);

#endif /* SEARCH_H */
