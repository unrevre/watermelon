#ifndef MEMORY_H
#define MEMORY_H

#include "magics.h"
#include "structs.h"

#include <stdint.h>

#define HASHBITS 24
#define HASHSIZE (0x1 << HASHBITS)
#define HASHMASK (HASHSIZE - 0x1)

#define BASKETS 4

#define exact 0x1
#define lower 0x2
#define upper 0x3

#define AGEMASK 0x3

extern move_t history[STEPLIMIT];
extern uint32_t htable[STEPLIMIT];
extern ttentry_t ttable[HASHSIZE];
extern killer_t ktable[PLYLIMIT];

/*!
 * reset_tables
 * @ reset tranposition/killer move tables
 */

void reset_tables(void);

/*!
 * store_hash
 * @ transposition table replacement scheme
 */

void store_hash(int32_t depth, int32_t alpha, int32_t beta, int32_t score,
                move_t move);

/*!
 * probe_hash
 * @ probe transposition table for current position
 */

int32_t probe_hash(int32_t depth, int32_t* alpha, int32_t* beta,
                   move_t* move);

/*!
 * probe_hash_for_entry
 * @ probe transposition table for current position and return first matching
 * entry
 */

ttentry_t probe_hash_for_entry();

/*!
 * advance_history
 * @ advance move history records
 */

void advance_history(move_t move);

/*!
 * undo_history
 * @ undo last move
 */

void undo_history();

/*!
 * redo_history
 * @ redo last undone move
 */

void redo_history();

#endif /* MEMORY_H */
