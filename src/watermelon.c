#include "fen.h"
#include "masks.h"
#include "perf.h"
#include "search.h"
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

   move_t move = iter_dfs(depth, 0);
   printf("%2i: %2i - %2i [%2i]\n", move.internal.pfrom,
      move.internal.from, move.internal.to, move.internal.pto);

   cpu_time = clock() - cpu_time;
   printf("cpu_time: %fs\n", (float)cpu_time / CLOCKS_PER_SEC);

#ifdef DEBUG
   char* fen_str = info_fen();
   printf("fen: %s\n", fen_str);
   free(fen_str);
   printf("nodes: %u\n", nodes);
   printf("qnodes: %u\n", qnodes);
#endif

   return 0;
}
