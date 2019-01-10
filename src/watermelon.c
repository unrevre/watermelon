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
            "9/P1P1P1P1P/1C5C1/9/RNBAKABNR r");
      case 3:
         return watermelon(atoi(argv[1]), argv[2]);
      default:
         printf("usage: %s [depth]\n", argv[0]);
         return 1;
   }
}

int watermelon(int32_t depth, char const* fen) {
   init_tables();

   init_masks();
   uint32_t side = init_fen(fen);
   init_hashes();

   init_state();

   printf("fen: %s\n", fen);

   clock_t cpu_time = clock();

   move_t move = iter_dfs(depth, side);

   cpu_time = clock() - cpu_time;
   printf("cpu_time: %fs\n\n", (float)cpu_time / CLOCKS_PER_SEC);

   info_game_state();
   info_move(move, '\n');
   printf("\n");

   trace(side);
   printf("\n");

#ifdef DEBUG
   debug_node_counts(nodes, qnodes, tthits);
   printf("\n");
#endif

   return 0;
}
