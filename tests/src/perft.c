#include "../../src/fen.h"
#include "../../src/masks.h"
#include "../../src/perf.h"
#include "../../src/state.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
   if (argc < 3) {
      printf("usage: %s [depth] [fen]\n", argv[0]);
      return 1;
   }

   int depth = atoi(argv[1]);

   init_masks();
   init_fen(argv[2]);

   printf("fen: %s\n", argv[2]);
   printf("perft:");
   for (int32_t i = 1; i < depth; ++i)
      printf(" %"PRIu64, perft(i, 0));
   printf("\n");

   return 0;
}
