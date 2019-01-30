#include "state.h"

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

uint32_t hash_state;

move_t history[TURNLIMIT];
uint32_t htable[8];
uint32_t step;

ttentry_t ttable[HASHSIZE] __attribute__((aligned(64)));
uint32_t age;

killer_t ktable[PLYLIMIT][2] __attribute__((aligned(64)));

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
         hash_state ^= PSHASH[i][bsf(piece)];
   }

   hash_state ^= rand();

   MVHASH = rand();
}

void init_tables(void) {
   memset(htable, 0, 8 * sizeof(uint32_t));
   memset(ttable, 0, HASHSIZE * sizeof(ttentry_t));
   memset(ktable, 0, PLYLIMIT * 2 * sizeof(killer_t));
}

void init_state(void) {
   age = 0;

   step = 0;
   htable[0] = hash_state;
}

void advance(move_t move) {
   hash_state ^= PSHASH[move._.pfrom][move._.from];
   hash_state ^= PSHASH[move._.pfrom][move._.to];
   hash_state ^= PSHASH[move._.pto][move._.to];
   hash_state ^= MVHASH;

   htable[++step & 0x7] = hash_state;

   game.pieces[move._.pfrom] ^= PMASK[move._.from] | PMASK[move._.to];
   game.pieces[move._.pto] ^= PMASK[move._.to];
   game.pieces[empty] ^= PMASK[move._.from];

   uint32_t s = s(move._.pfrom);
   game.occupancy[s] ^= PMASK[move._.from] | PMASK[move._.to];
   game.occupancy[!s] ^= game.occupancy[0] & game.occupancy[1];

   board[move._.from] = empty;
   board[move._.to] = move._.pfrom;
}

void retract(move_t move) {
   hash_state ^= PSHASH[move._.pfrom][move._.from];
   hash_state ^= PSHASH[move._.pfrom][move._.to];
   hash_state ^= PSHASH[move._.pto][move._.to];
   hash_state ^= MVHASH;

   --step;

   game.pieces[move._.pfrom] ^= PMASK[move._.from] | PMASK[move._.to];
   game.pieces[move._.pto] ^= PMASK[move._.to];
   game.pieces[empty] ^= PMASK[move._.from];

   uint32_t s = s(move._.pfrom);
   game.occupancy[s] ^= PMASK[move._.from] | PMASK[move._.to];
   game.occupancy[!s] ^= (game.pieces[empty] & PMASK[move._.to])
      ^ PMASK[move._.to];

   board[move._.from] = move._.pfrom;
   board[move._.to] = move._.pto;
}
