#ifndef STATE_H
#define STATE_H

#include "structs.h"

#include <stdint.h>

extern state_t game __attribute__((aligned(64)));

extern uint32_t board[128] __attribute__((aligned(64)));

extern uint32_t PSHASH[15][128] __attribute__((aligned(64)));
extern uint32_t MVHASH;

extern transient_t state;

/*!
 * init_hashes
 * @ initialise zobrist hashes
 */

void init_hashes(void);

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

/*!
 * is_legal
 * @ test if given move is legal in current position
 */

uint32_t is_legal(move_t move);

/*!
 * is_repetition
 * @ test if current position is in state of repetition
 */

uint32_t is_repetition();

#endif /* STATE_H */
