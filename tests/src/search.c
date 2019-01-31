#include "../../src/debug.h"
#include "../../src/perf.h"
#include "../../src/search.h"
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

   uint32_t depth = atoi(argv[1]);

   init_state(argv[2]);

   printf("fen: %s\n", argv[2]);
   move_t move = iter_dfs(depth);
   info_move(move, ' ');
   printf("at depth %i\n", depth);

   trace();
   printf("\n");

   return 0;
}
