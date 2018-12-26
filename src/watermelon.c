#include "fen.h"
#include "masks.h"
#include "perf.h"
#include "state.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*!
 * main
 * @ perft
 */

int main(int argc, char const* argv[]) {
   if (argc < 2) {
      printf("usage: ./watermelon [depth]\n");
      return 1;
   }

   init_masks();
   init_fen("rnbakabnr/9/1c5c1/p1p1p1p1p/9/"
            "9/P1P1P1P1P/1C5C1/9/RNBAKABNR");

   int depth = atoi(argv[1]);

   clock_t cpu_time = clock();

   for (int32_t i = 1; i < depth; ++i)
      printf("perft: %"PRIu64", at depth: %i\n", perft(i, 0), i);

   cpu_time = clock() - cpu_time;
   printf("cpu_time: %fs\n", (float)cpu_time / CLOCKS_PER_SEC);

   return 0;
}
