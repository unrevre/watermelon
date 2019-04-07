#include "state.h"

#include "fen.h"
#include "generate.h"
#include "magics.h"
#include "masks.h"
#include "memory.h"
#include "search.h"

#include <stdlib.h>
#include <string.h>

state_t game __attribute__((aligned(64)));
uint32_t board[128] __attribute__((aligned(64)));

uint32_t PSHASH[15][128] __attribute__((aligned(64)));
uint32_t STHASH;
uint32_t MVHASH;

search_t search;
transient_t state;
int64_t age;

void init_hashes(void) {
   srand(0x91);

   for (int64_t i = 0; i != empty; ++i)
      for (int64_t j = 0; j != 90; ++j)
         PSHASH[i][j] = rand();
   for (int64_t i = 0; i != 128; ++i)
      PSHASH[empty][i] = 0x0;

   STHASH = rand();
   MVHASH = rand();
}

void reset_hashes(void) {
   for (int64_t i = 0; i != 90; ++i)
      if (board[i] != empty)
         state.hash ^= PSHASH[board[i]][i];

   state.hash ^= STHASH;
   htable[0] = state.hash;
}

void init_search(void) {
   search.clock = malloc(sizeof(timer_t));
   search.clock->status = 1;
   search.clock->limit = -1.;
}

void reset_search(void) {
   search.clock->status = 0;

   search.nodes = 0;
   search.qnodes = 0;
   search.tthits = 0;
}

void set_timer(double time) {
   search.clock->limit = time;
}

void init_state(const char* fen) {
   init_hashes();
   init_masks();
   init_search();

   reset_state(fen);
}

void reset_state(const char* fen) {
   game = (state_t){ {0}, {0} };
   state = (transient_t){ 0, 0, 0, 0 };
   age = 0;

   reset_fen(fen);
   reset_hashes();
   reset_search();
   reset_tables();
}

void advance(move_t move) {
   ++state.ply;
   state.side = !state.side;

   state.hash ^= PSHASH[move._.pfrom][move._.from];
   state.hash ^= PSHASH[move._.pfrom][move._.to];
   state.hash ^= PSHASH[move._.pto][move._.to];
   state.hash ^= MVHASH;

   htable[++state.step] = state.hash;

   game.pieces[move._.pfrom] ^= PMASK[move._.from] ^ PMASK[move._.to];
   game.pieces[move._.pto] ^= PMASK[move._.to];
   game.pieces[empty] ^= PMASK[move._.from];

   uint32_t s = s(move._.pfrom);
   game.occupancy[s] ^= PMASK[move._.from] ^ PMASK[move._.to];
   game.occupancy[!s] ^= game.occupancy[0] & game.occupancy[1];

   board[move._.from] = empty;
   board[move._.to] = move._.pfrom;
}

void retract(move_t move) {
   --state.ply;
   state.side = !state.side;

   state.hash ^= PSHASH[move._.pfrom][move._.from];
   state.hash ^= PSHASH[move._.pfrom][move._.to];
   state.hash ^= PSHASH[move._.pto][move._.to];
   state.hash ^= MVHASH;

   --state.step;

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

void advance_game(move_t move) {
   advance(move);
   state.ply = 0;
   ++age;
}

void retract_game(move_t move) {
   retract(move);
   state.ply = 0;
   --age;
}
