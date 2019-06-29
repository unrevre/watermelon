#ifndef MEMORY_H
#define MEMORY_H

#include "structs.h"

#include <stdint.h>

#define HASHBITS 24
#define HASHSIZE (0x1 << HASHBITS)
#define HASHMASK (HASHSIZE - 0x1)

extern union ttentry_t ttable[HASHSIZE];

/*!
 * store_hash
 * @ transposition table replacement scheme
 */

void store_hash(struct transient_t* state, int32_t depth, int32_t alpha,
                int32_t beta, int32_t score, union move_t move);

/*!
 * probe_hash
 * @ probe transposition table for current position
 */

int32_t probe_hash(struct transient_t* state, int32_t depth, int32_t* alpha,
                   int32_t* beta, union move_t* move);

/*!
 * entry_for_state
 * @ probe transposition table for entry corresponding to current state
 */

union ttentry_t entry_for_state(struct transient_t* state);

/*!
 * move_for_state
 * @ probe transposition table for hash move corresponding to current state
 */

union move_t move_for_state(struct transient_t* state);

#endif /* MEMORY_H */
