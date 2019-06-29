#include "state.h"

#include "fen.h"
#include "magics.h"
#include "masks.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static uint32_t PSHASH[PIECES][BITS] __attribute__((aligned(64)));
static uint32_t MVHASH;

struct transient_t trunk;

void init_hashes(void) {
   srand(0x91);

   for (int64_t i = 0; i != empty; ++i)
      for (int64_t j = 0; j != POINTS; ++j)
         PSHASH[i][j] = rand();
   for (int64_t i = 0; i != POINTS; ++i)
      PSHASH[empty][i] = 0x0;

   MVHASH = rand();
}

/*!
 * reset_hashes
 * @ reset zobrist hash
 */

static void reset_hashes(void) {
   trunk.hash = rand();
   for (int64_t i = 0; i != POINTS; ++i)
      if (trunk.board[i] != empty)
         trunk.hash ^= PSHASH[trunk.board[i]][i];

   trunk.hashes[0] = trunk.hash;
}

void set_state(const char* fen) {
   memset(&trunk, 0, sizeof(struct transient_t));

   reset_fen(fen);
   reset_hashes();
}

/*!
 * advance_state
 * @ advance move (update state variables)
 */

static void advance_state(union move_t move, struct transient_t* state) {
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

static void retract_state(union move_t move, struct transient_t* state) {
   --state->ply;
   state->side = !state->side;

   state->hash ^= PSHASH[move._.pfrom][move._.from];
   state->hash ^= PSHASH[move._.pfrom][move._.to];
   state->hash ^= PSHASH[move._.pto][move._.to];
   state->hash ^= MVHASH;
}

void advance_board(union move_t move, struct transient_t* state) {
   state->pieces[move._.pfrom] ^= PMASK[move._.from] ^ PMASK[move._.to];
   state->pieces[move._.pto] ^= PMASK[move._.to];
   state->pieces[empty] ^= PMASK[move._.from];

   uint32_t s = s(move._.pfrom);
   state->occupancy[s] ^= PMASK[move._.from] ^ PMASK[move._.to];
   state->occupancy[!s] ^= state->occupancy[0] & state->occupancy[1];

   state->board[move._.from] = empty;
   state->board[move._.to] = move._.pfrom;
}

void retract_board(union move_t move, struct transient_t* state) {
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

void advance(union move_t move, struct transient_t* state) {
   advance_state(move, state);
   state->hashes[trunk.ply + state->ply] = state->hash;
   advance_board(move, state);
}

void retract(union move_t move, struct transient_t* state) {
   retract_state(move, state);
   retract_board(move, state);
}

void advance_game(union move_t move) {
   advance_state(move, &trunk);
   trunk.hashes[trunk.ply] = trunk.hash;
   advance_board(move, &trunk);
}

void retract_game(union move_t move) {
   retract_state(move, &trunk);
   retract_board(move, &trunk);
}
