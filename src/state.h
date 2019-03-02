#ifndef STATE_H
#define STATE_H

#include "structs.h"

#include <stdint.h>

extern state_t game;

extern uint32_t board[128];

extern uint32_t PSHASH[15][128];
extern uint32_t MVHASH;

extern transient_t state;
extern int64_t age;

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
 * advance_game
 * @ advance move and reset search state
 */

void advance_game(move_t move);

/*!
 * retract_game
 * @ retract move and reset search state
 */

void retract_game(move_t move);

#endif /* STATE_H */
