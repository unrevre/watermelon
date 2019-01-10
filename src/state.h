#ifndef STATE_H
#define STATE_H

#include "structs.h"

#include <stdint.h>

extern state_t game __attribute__((aligned(64)));

extern uint32_t board[90];

extern uint32_t PSHASH[15][90];
extern uint32_t MVHASH;

extern uint32_t htable[8];

extern uint32_t step;

extern uint32_t hash_state;

extern ttentry_t ttable[0x1000000] __attribute__((aligned(64)));

extern uint32_t age;

extern killer_t ktable[32][2] __attribute__((aligned(64)));

extern char fen_rep[15];

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

void init_state(void);

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
