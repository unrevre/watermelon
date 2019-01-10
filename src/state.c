#include "state.h"

#include "inlines.h"
#include "masks.h"
#include "structs.h"

#include <stdlib.h>
#include <string.h>

state_t game = {{0x0}, 0x0, {0x0}};

uint32_t board[90] = {
   0x0, 0x1, 0x4, 0x5, 0x6, 0x5, 0x4, 0x1, 0x0,
   0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7,
   0x7, 0x2, 0x7, 0x7, 0x7, 0x7, 0x7, 0x2, 0x7,
   0x3, 0x7, 0x3, 0x7, 0x3, 0x7, 0x3, 0x7, 0x3,
   0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7,
   0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7,
   0xb, 0x7, 0xb, 0x7, 0xb, 0x7, 0xb, 0x7, 0xb,
   0x7, 0xa, 0x7, 0x7, 0x7, 0x7, 0x7, 0xa, 0x7,
   0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7,
   0x8, 0x9, 0xc, 0xd, 0xe, 0xd, 0xc, 0x9, 0x8
};

uint32_t PSHASH[15][90];
uint32_t MVHASH;

uint32_t htable[8];

uint32_t step;

uint32_t hash_state;

ttentry_t ttable[0x1000000] __attribute__((aligned(64)));

uint32_t age;

killer_t ktable[32][2] __attribute__((aligned(64)));

void init_hashes(void) {
   srand(0x91);

   for (uint32_t i = 0; i < 15; ++i)
      for (uint32_t j = 0; j < 90; ++j)
         PSHASH[i][j] = rand();

   for (uint32_t i = 0; i < 90; ++i)
      PSHASH[7][i] = 0x0;

   for (uint32_t i = 0; i < 15; ++i) {
      __uint128_t piece = game.pieces[i];
      for (; piece; piece &= piece - 1)
         hash_state ^= PSHASH[i][bsf(piece)];
   }

   uint32_t hash_red = rand();
   uint32_t hash_black = rand();

   MVHASH = hash_red ^ hash_black;

   hash_state ^= hash_red;
}

void init_tables(void) {
   memset(htable, 0, 8 * sizeof(uint32_t));
   memset(ttable, 0, 0x1000000 * sizeof(ttentry_t));
   memset(ktable, 0, 32 * 2 * sizeof(killer_t));
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

   uint32_t s = move._.pfrom >> 3;
   game.pieces[move._.pfrom] ^= PMASK[move._.from] | PMASK[move._.to];
   game.pieces[move._.pto] ^= PMASK[move._.to];
   game.pieces[0x7] = 0x0;

   game.occupancy[s] ^= PMASK[move._.from] | PMASK[move._.to];
   game.occupancy[!s] ^= game.occupancy[0] & game.occupancy[1];
   game.empty = ~(game.occupancy[0] | game.occupancy[1]);

   board[move._.from] = 0x7;
   board[move._.to] = move._.pfrom;
}

void retract(move_t move) {
   hash_state ^= PSHASH[move._.pfrom][move._.from];
   hash_state ^= PSHASH[move._.pfrom][move._.to];
   hash_state ^= PSHASH[move._.pto][move._.to];
   hash_state ^= MVHASH;

   --step;

   uint32_t s = move._.pfrom >> 3;
   game.pieces[move._.pfrom] ^= PMASK[move._.from] | PMASK[move._.to];
   game.pieces[move._.pto] ^= PMASK[move._.to];
   game.pieces[0x7] = 0x0;

   game.occupancy[s] ^= PMASK[move._.from] | PMASK[move._.to];
   game.occupancy[!s] |= game.pieces[move._.pto];
   game.empty = ~(game.occupancy[0] | game.occupancy[1]);

   board[move._.from] = move._.pfrom;
   board[move._.to] = move._.pto;
}
