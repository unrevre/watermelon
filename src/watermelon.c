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
   init_masks();
   init_hashes();
   init_tables();
   init_variables();
   init_fen(fen);

   clock_t cpu_time = clock();

   move_t move = iter_dfs(depth, 0);
   printf("%c: %2i - %2i [%c]\n", fen_rep[move._.pfrom],
      move._.from, move._.to, fen_rep[move._.pto]);

   cpu_time = clock() - cpu_time;
   printf("cpu_time: %fs\n", (float)cpu_time / CLOCKS_PER_SEC);

   trace(move, 0);

#ifdef DEBUG
   char* fen_str = info_fen();
   printf("fen: %s\n", fen_str);
   free(fen_str);
   printf("nodes: %u\n", nodes);
   printf("qnodes: %u\n", qnodes);
   printf("tthits: %u\n", tthits);
#endif

   return 0;
}
