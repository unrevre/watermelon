#include "state.h"

#include "fen.h"
#include "masks.h"

#include <stdlib.h>
#include <string.h>

uint32_t PSHASH[PIECES][BITS] __attribute__((aligned(64)));
uint32_t STHASH;
uint32_t MVHASH;

transient_t trunk;

move_t history[STEPLIMIT];

void init_hashes(void) {
   srand(0x91);

   for (int64_t i = 0; i != empty; ++i)
      for (int64_t j = 0; j != POINTS; ++j)
         PSHASH[i][j] = rand();
   for (int64_t i = 0; i != POINTS; ++i)
      PSHASH[empty][i] = 0x0;

   STHASH = rand();
   MVHASH = rand();
}

/*!
 * reset_hashes
 * @ reset zobrist hash
 */

void reset_hashes(void) {
   for (int64_t i = 0; i != POINTS; ++i)
      if (trunk.board[i] != empty)
         trunk.hash ^= PSHASH[trunk.board[i]][i];

   trunk.hash ^= STHASH;
   trunk.hashes[0] = trunk.hash;
}

void set_state(const char* fen) {
   memset(&trunk, 0, sizeof(transient_t));
   memset(history, 0, STEPLIMIT * sizeof(move_t));

   reset_fen(fen);
   reset_hashes();
}

/*!
 * advance_state
 * @ advance move (update state variables)
 */

void advance_state(move_t move, transient_t* state) {
   ++state->ply;
   state->side = !state->side;

   state->hash ^= PSHASH[move._.pfrom][move._.from];
   state->hash ^= PSHASH[move._.pfrom][move._.to];
   state->hash ^= PSHASH[move._.pto][move._.to];
   state->hash ^= MVHASH;
}

/*!
 * retract_state
 * @ retract move (update state variables)
 */

void retract_state(move_t move, transient_t* state) {
   --state->ply;
   state->side = !state->side;

   state->hash ^= PSHASH[move._.pfrom][move._.from];
   state->hash ^= PSHASH[move._.pfrom][move._.to];
   state->hash ^= PSHASH[move._.pto][move._.to];
   state->hash ^= MVHASH;
}

void advance_board(move_t move, transient_t* state) {
   state->pieces[move._.pfrom] ^= PMASK[move._.from] ^ PMASK[move._.to];
   state->pieces[move._.pto] ^= PMASK[move._.to];
   state->pieces[empty] ^= PMASK[move._.from];

   uint32_t s = s(move._.pfrom);
   state->occupancy[s] ^= PMASK[move._.from] ^ PMASK[move._.to];
   state->occupancy[!s] ^= state->occupancy[0] & state->occupancy[1];

   state->board[move._.from] = empty;
   state->board[move._.to] = move._.pfrom;
}

void retract_board(move_t move, transient_t* state) {
   state->pieces[move._.pfrom] ^= PMASK[move._.from] ^ PMASK[move._.to];
   state->pieces[move._.pto] ^= PMASK[move._.to];
   state->pieces[empty] ^= PMASK[move._.from];

   uint32_t s = s(move._.pfrom);
   state->occupancy[s] ^= PMASK[move._.from] ^ PMASK[move._.to];
   state->occupancy[!s] ^= (state->pieces[empty] & PMASK[move._.to])
      ^ PMASK[move._.to];

   state->board[move._.from] = move._.pfrom;
   state->board[move._.to] = move._.pto;
}

void advance(move_t move, transient_t* state) {
   advance_state(move, state);
   state->hashes[trunk.ply + state->ply] = state->hash;
   advance_board(move, state);
}

void retract(move_t move, transient_t* state) {
   retract_state(move, state);
   retract_board(move, state);
}

void advance_game(move_t move) {
   advance_state(move, &trunk);
   trunk.hashes[trunk.ply] = trunk.hash;
   advance_board(move, &trunk);
}

void retract_game(move_t move) {
   retract_state(move, &trunk);
   retract_board(move, &trunk);
}

void advance_history(move_t move) {
   int32_t step = trunk.ply;
   move_t future = history[step];
   if (future.bits && move.bits != future.bits)
      while (history[++step].bits)
         history[step] = (move_t){0};

   history[trunk.ply] = move;
}

void undo_history(void) {
   if (trunk.ply) { retract_game(history[trunk.ply - 1]); }
}

void redo_history(void) {
   if (history[trunk.ply].bits) {
      advance_history(history[trunk.ply]);
      advance_game(history[trunk.ply]);
   }
}
