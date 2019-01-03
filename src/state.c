#include "state.h"

#include "inlines.h"
#include "structs.h"

#include <stdlib.h>
#include <string.h>

state_t GAME = {{0x0}, 0x0, {0x0}};

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

uint32_t hashes[15][90];
uint32_t hash_move;
uint32_t hash_state;

ttentry_t TTABLE[0x1000000] __attribute__((aligned(64)));

uint32_t age;

killer_t KTABLE[32][2] __attribute__((aligned(64)));

void init_hashes(void) {
   srand(0x91);

   for (uint32_t i = 0; i < 15; ++i)
      for (uint32_t j = 0; j < 90; ++j)
         hashes[i][j] = rand();

   for (uint32_t i = 0; i < 90; ++i)
      hashes[7][i] = 0x0;

   for (uint32_t i = 0; i < 15; ++i) {
      __uint128_t piece = GAME.pieces[i];
      for (; piece; piece &= piece - 1)
         hash_state ^= hashes[i][bsf(piece)];
   }

   uint32_t hash_red = rand();
   uint32_t hash_black = rand();

   hash_move = hash_red ^ hash_black;

   hash_state ^= hash_red;
}

void init_tables(void) {
   memset(TTABLE, 0, 0x1000000 * sizeof(ttentry_t));
   memset(KTABLE, 0, 32 * 2 * sizeof(killer_t));
}

void init_variables(void) {
   age = 0;
}
