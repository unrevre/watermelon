#include "../../src/debug.h"
#include "../../src/fen.h"
#include "../../src/masks.h"
#include "../../src/perf.h"
#include "../../src/search.h"
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

   init_tables();

   init_masks();
   init_fen(argv[2]);
   init_hashes();

   init_state();

   printf("fen: %s\n", argv[2]);
   move_t move = iter_dfs(depth, 0);
   info_move(move, ' ');
   printf("at depth %i\n", depth);

   trace(0);

   return 0;
}
