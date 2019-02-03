#ifndef MEMORY_H
#define MEMORY_H

#include "magics.h"
#include "structs.h"

#include <stdint.h>

extern move_t history[TURNLIMIT];
extern uint32_t htable[8];

extern ttentry_t ttable[HASHSIZE] __attribute__((aligned(64)));
extern uint32_t age;

extern killer_t ktable[PLYLIMIT][2] __attribute__((aligned(64)));

/*!
 * init_tables
 * @ initialise tranposition/killer move tables
 */

void init_tables(void);

/*!
 * store_hash
 * @ transposition table replacement scheme
 */

void store_hash(uint32_t depth, int32_t alpha, int32_t beta, int32_t score,
                move_t move);

/*!
 * probe_hash
 * @ probe transposition table for current position
 */

int32_t probe_hash(uint32_t depth, int32_t* alpha, int32_t* beta,
                   move_t* move);

#endif /* MEMORY_H */