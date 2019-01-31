#ifndef STATE_H
#define STATE_H

#include "magics.h"
#include "structs.h"

#include <stdint.h>

extern state_t game __attribute__((aligned(64)));

extern uint32_t board[128] __attribute__((aligned(64)));

extern uint32_t PSHASH[15][128] __attribute__((aligned(64)));
extern uint32_t MVHASH;

extern move_t history[TURNLIMIT];
extern uint32_t htable[8];

extern ttentry_t ttable[HASHSIZE] __attribute__((aligned(64)));
extern uint32_t age;

extern killer_t ktable[PLYLIMIT][2] __attribute__((aligned(64)));

extern transient_t state;

/*!
 * init_hashes
 * @ initialise zobrist hashes
 */

void init_hashes(void);

/*!
 * init_tables
 * @ initialise tranposition/killer move tables
 */

void init_tables(void);

/*!
 * init_state
 * @ initialise global state variables
 */

void init_state(const char* fen);

/*!
 * advance
 * @ advance move
 */

void advance(move_t move);

/*!
 * retract
 * @ retract move
 */

void retract(move_t move);

#endif /* STATE_H */
