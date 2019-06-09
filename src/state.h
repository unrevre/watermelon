#ifndef STATE_H
#define STATE_H

#include "magics.h"
#include "structs.h"

#include <stdint.h>

extern struct transient_t trunk;

extern union move_t history[STEPLIMIT];
extern uint32_t htable[STEPLIMIT];

/*!
 * init_hashes
 * @ initialise zobrist hashes
 */

void init_hashes(void);

/*!
 * set_state
 * @ set global state variables
 */

void set_state(const char* fen);

/*!
 * advance_board
 * @ advance move (update piece bitboards and board array)
 */

void advance_board(union move_t move, struct transient_t* state);

/*!
 * retract_board
 * @ retract move (update piece bitboards and board array)
 */

void retract_board(union move_t move, struct transient_t* state);

/*!
 * advance
 * @ advance move
 */

void advance(union move_t move, struct transient_t* state);

/*!
 * retract
 * @ retract move
 */

void retract(union move_t move, struct transient_t* state);

/*!
 * advance_game
 * @ advance move and reset search state
 */

void advance_game(union move_t move);

/*!
 * retract_game
 * @ retract move and reset search state
 */

void retract_game(union move_t move);

/*!
 * advance_history
 * @ advance move history records
 */

void advance_history(union move_t move);

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

#endif /* STATE_H */
