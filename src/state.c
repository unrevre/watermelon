#include "state.h"

#include "fen.h"
#include "inlines.h"
#include "magics.h"
#include "masks.h"
#include "structs.h"

#include <stdlib.h>
#include <string.h>

state_t game = {{0x0}, {0x0}};

uint32_t board[128] __attribute__((aligned(64)));

uint32_t PSHASH[15][128] __attribute__((aligned(64)));
uint32_t MVHASH;

move_t history[TURNLIMIT];
uint32_t htable[8];

ttentry_t ttable[HASHSIZE] __attribute__((aligned(64)));
uint32_t age;

killer_t ktable[PLYLIMIT][2] __attribute__((aligned(64)));

transient_t state;

void init_hashes(void) {
   srand(0x91);

   for (uint32_t i = 0x0; i != 0xe; ++i)
      for (uint32_t j = 0; j < 90; ++j)
         PSHASH[i][j] = rand();
   for (uint32_t i = 0; i < 128; ++i)
      PSHASH[empty][i] = 0x0;

   for (uint32_t i = 0x0; i != 0xe; ++i) {
      __uint128_t piece = game.pieces[i];
      for (; piece; piece &= piece - 1)
         state.hash ^= PSHASH[i][bsf(piece)];
   }

   state.hash ^= rand();

   MVHASH = rand();

   htable[0] = state.hash;
}

void init_tables(void) {
   memset(htable, 0, 8 * sizeof(uint32_t));
   memset(ttable, 0, HASHSIZE * sizeof(ttentry_t));
   memset(ktable, 0, PLYLIMIT * 2 * sizeof(killer_t));

   age = 0;
}

void init_state(const char* fen) {
   state = (transient_t){ 0, 0, 0, 0 };

   init_tables();
   init_masks();

   init_fen(fen);

   init_hashes();
}

void advance(move_t move) {
   ++state.ply;
   state.side = o(state.side);

   state.hash ^= PSHASH[move._.pfrom][move._.from];
   state.hash ^= PSHASH[move._.pfrom][move._.to];
   state.hash ^= PSHASH[move._.pto][move._.to];
   state.hash ^= MVHASH;

   htable[++state.step & 0x7] = state.hash;

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
   state.side = o(state.side);

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
