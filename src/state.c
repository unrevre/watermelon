#include "state.h"

#include "debug.h"
#include "fen.h"
#include "generate.h"
#include "masks.h"
#include "memory.h"
#include "search.h"

#include <stdlib.h>
#include <string.h>

state_t game __attribute__((aligned(64)));
uint32_t board[BITS] __attribute__((aligned(64)));

uint32_t PSHASH[PIECES][BITS] __attribute__((aligned(64)));
uint32_t STHASH;
uint32_t MVHASH;

search_t search;
transient_t trunk;

/*!
 * init_hashes
 * @ initialise zobrist hashes
 */

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
      if (board[i] != empty)
         trunk.hash ^= PSHASH[board[i]][i];

   trunk.hash ^= STHASH;
   htable[0] = trunk.hash;
}

void reset_search(transient_t* state) {
   search.clock->status = 0;

   search.nodes = 0;
   search.qnodes = 0;
   search.tthits = 0;

   state->hash = trunk.hash;
   state->ply = 0;
   state->side = trunk.side;

   tree_debug_state(state);
}

void set_timer(double time) {
   search.clock->limit = time;
}

void init_state(const char* fen) {
   init_hashes();

   reset_state(fen);

   search.clock = malloc(sizeof(wmclock_t));
   search.clock->status = 1;
   search.clock->limit = -1.;
}

void reset_state(const char* fen) {
   game = (state_t){ {0}, {0} };
   trunk = (transient_t){ 0, 0, 0 };

   reset_fen(fen);
   reset_hashes();
   reset_tables();
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

   htable[trunk.ply + state->ply] = state->hash;
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

void advance_board(move_t move) {
   game.pieces[move._.pfrom] ^= PMASK[move._.from] ^ PMASK[move._.to];
   game.pieces[move._.pto] ^= PMASK[move._.to];
   game.pieces[empty] ^= PMASK[move._.from];

   uint32_t s = s(move._.pfrom);
   game.occupancy[s] ^= PMASK[move._.from] ^ PMASK[move._.to];
   game.occupancy[!s] ^= game.occupancy[0] & game.occupancy[1];

   board[move._.from] = empty;
   board[move._.to] = move._.pfrom;
}

void retract_board(move_t move) {
   game.pieces[move._.pfrom] ^= PMASK[move._.from] ^ PMASK[move._.to];
   game.pieces[move._.pto] ^= PMASK[move._.to];
   game.pieces[empty] ^= PMASK[move._.from];

   uint32_t s = s(move._.pfrom);
   game.occupancy[s] ^= PMASK[move._.from] ^ PMASK[move._.to];
   game.occupancy[!s] ^= (game.pieces[empty] & PMASK[move._.to])
      ^ PMASK[move._.to];

   board[move._.from] = move._.pfrom;
   board[move._.to] = move._.pto;
}

void advance(move_t move, transient_t* state) {
   advance_state(move, state);
   advance_board(move);
}

void retract(move_t move, transient_t* state) {
   retract_state(move, state);
   retract_board(move);
}

void advance_game(move_t move) {
   trunk.side = !trunk.side;

   trunk.hash ^= PSHASH[move._.pfrom][move._.from];
   trunk.hash ^= PSHASH[move._.pfrom][move._.to];
   trunk.hash ^= PSHASH[move._.pto][move._.to];
   trunk.hash ^= MVHASH;

   htable[++trunk.ply] = trunk.hash;

   advance_board(move);
}

void retract_game(move_t move) {
   trunk.side = !trunk.side;

   trunk.hash ^= PSHASH[move._.pfrom][move._.from];
   trunk.hash ^= PSHASH[move._.pfrom][move._.to];
   trunk.hash ^= PSHASH[move._.pto][move._.to];
   trunk.hash ^= MVHASH;

   --trunk.ply;

   retract_board(move);
}
