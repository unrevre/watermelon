#ifndef MEMORY_H
#define MEMORY_H

#include "magics.h"
#include "structs.h"

#include <stdint.h>

#define STEPLIMIT 0x100

#define HASHBITS 24
#define HASHSIZE (0x1 << HASHBITS)
#define HASHMASK  (HASHSIZE - 0x1)

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

void store_hash(transient_t* state, int32_t depth, int32_t alpha, int32_t beta,
                int32_t score, move_t move);

/*!
 * probe_hash
 * @ probe transposition table for current position
 */

int32_t probe_hash(transient_t* state, int32_t depth, int32_t* alpha,
                   int32_t* beta, move_t* move);

/*!
 * entry_for_state
 * @ probe transposition table for entry corresponding to current state
 */

ttentry_t entry_for_state(transient_t* state);

/*!
 * move_for_state
 * @ probe transposition table for hash move corresponding to current state
 */

move_t move_for_state(transient_t* state);

/*!
 * advance_history
 * @ advance move history records
 */

void advance_history(move_t move);

/*!
 * undo_history
 * @ undo last move
 */

void undo_history(void);

/*!
 * redo_history
 * @ redo last undone move
 */

void redo_history(void);

#endif /* MEMORY_H */
