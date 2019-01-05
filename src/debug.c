#include "debug.h"

#include "fen.h"

#include <stdio.h>
#include <stdlib.h>

static char cside[2] = {'r', 'b'};

void display(__uint128_t bits) {
   uint32_t b[90] = {0};

   __uint128_t bitmask = 0x1;
   for (uint32_t i = 0; i < 90; ++i) {
      if (bits & bitmask)
         b[i] = 1;
      bitmask = bitmask << 1;
   }

   for (uint32_t j = 10; j > 0; --j) {
      for (uint32_t k = 0; k < 9; ++k)
         printf("%i ", b[9 * (j - 1) + k]);
      printf("\n");
   }

   printf("\n");
}

void tree_node_exit(uint32_t ply, int32_t alpha, int32_t beta, int32_t score,
                    uint32_t side) {
   for (uint32_t t = 0; t < ply + 1; ++t) { printf("│"); }
   char* fen = info_fen();
   printf("├╸(%c) %s\n", cside[!side], fen);
   for (uint32_t t = 0; t < ply + 1; ++t) { printf("│"); }
   printf("└╸%5i [%5i, %5i]\n", -score, -beta, -alpha);
   free(fen);
}

void tree_node_entry(uint32_t ply, int32_t alpha, int32_t beta, uint32_t side) {
   for (uint32_t t = 0; t < ply - 1; ++t) { printf("│"); }
   char* fen = info_fen();
   printf("├┬╸(%c) %s\n", cside[side >> 3], fen);
   for (uint32_t t = 0; t < ply; ++t) { printf("│"); }
   printf("├╸      [%5i, %5i]\n", alpha, beta);
   free(fen);
}
