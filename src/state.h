#ifndef STATE_H
#define STATE_H

#include "magics.h"
#include "structs.h"

#include <stdint.h>

extern state_t game;

extern uint32_t PSHASH[PIECES][BITS];
extern uint32_t MVHASH;

extern search_t search;
extern transient_t trunk;

/*!
 * reset_search
 * @ reset search parameters
 */

void reset_search(transient_t* state);

/*!
 * set_timer
 * @ set move time limit
 */

void set_timer(double time);

/*!
 * init_state
 * @ initialise global state variables
 */

void init_state(const char* fen);

/*!
 * reset_state
 * @ (re)set global state variables
 */

void reset_state(const char* fen);

/*!
 * advance_board
 * @ advance move (update piece bitboards and board array)
 */

void advance_board(move_t move);

/*!
 * retract_board
 * @ retract move (update piece bitboards and board array)
 */

void retract_board(move_t move);

/*!
 * advance
 * @ advance move
 */

void advance(move_t move, transient_t* state);

/*!
 * retract
 * @ retract move
 */

void retract(move_t move, transient_t* state);

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
