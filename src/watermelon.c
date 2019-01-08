#include "debug.h"
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

int watermelon(int32_t depth, char const* fen);

int main(int argc, char const* argv[]) {

   switch (argc) {
      case 2:
         return watermelon(atoi(argv[1]),
            "rnbakabnr/9/1c5c1/p1p1p1p1p/9/"
            "9/P1P1P1P1P/1C5C1/9/RNBAKABNR");
      case 3:
         return watermelon(atoi(argv[1]), argv[2]);
      default:
         printf("usage: %s [depth]\n", argv[0]);
         return 1;
   }
}

int watermelon(int32_t depth, char const* fen) {
   init_tables();
   init_variables();

   init_masks();
   init_fen(fen);
   init_hashes();

   printf("fen: %s\n", fen);

   clock_t cpu_time = clock();

   move_t move = iter_dfs(depth, 0);

   cpu_time = clock() - cpu_time;
   printf("cpu_time: %fs\n\n", (float)cpu_time / CLOCKS_PER_SEC);

   info_game_state();
   info_move(move, '\n');
   printf("\n");

   trace(0);
   printf("\n");

#ifdef DEBUG
   printf("nodes: %u\n", nodes);
   printf("qnodes: %u\n", qnodes);
   printf("tthits: %u\n", tthits);
#endif

   return 0;
}
