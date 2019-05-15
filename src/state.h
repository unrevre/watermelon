#ifndef STATE_H
#define STATE_H

#include "magics.h"
#include "structs.h"

#include <stdint.h>

extern uint32_t PSHASH[PIECES][BITS];
extern uint32_t MVHASH;

extern search_t search;
extern transient_t trunk;

/*!
 * initialise
 * @ initialise global state variables
 */

void initialise(const char* fen);

/*!
 * terminate
 * @ destroy global state variables
 */

void terminate(void);

/*!
 * set_state
 * @ set global state variables
 */

void set_state(const char* fen);

/*!
 * set_timer
 * @ set move time limit
 */

void set_timer(double time);

/*!
 * set_search
 * @ reset search parameters
 */

void set_search(transient_t* state);

/*!
 * advance_board
 * @ advance move (update piece bitboards and board array)
 */

void advance_board(move_t move, transient_t* state);

/*!
 * retract_board
 * @ retract move (update piece bitboards and board array)
 */

void retract_board(move_t move, transient_t* state);

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
