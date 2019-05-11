#include "../../src/perf.h"
#include "../../src/state.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
   if (argc != 3) {
      printf("usage: %s [depth] [fen]\n", argv[0]);
      return 1;
   }

   initialise(argv[2]);

   printf("fen: %s\n", argv[2]);
   printf("perft:");
   for (int32_t i = 1; i < atoi(argv[1]); ++i)
      printf(" %"PRIi64, perft(i));
   printf("\n");

   terminate();

   return 0;
}
