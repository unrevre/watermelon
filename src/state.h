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
 * advance_with_history
 * @ advance move with record in history table
 */

void advance_with_history(move_t move);

/*!
 * retract_with_history
 * @ retract move with record in history table
 */

void retract_with_history(move_t move);

/*!
 * undo_history
 * @ undo last move
 */

void undo_history();

/*!
 * redo_history
 * @ redo last undone move, if possible
 */

void redo_history();

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

/*!
 * is_index_movable
 * @ test if piece at index is on side to move
 */

uint32_t is_index_movable(uint32_t index);

#endif /* STATE_H */
