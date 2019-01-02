#include "../../src/fen.h"
#include "../../src/masks.h"
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

   init_masks();
   init_hashes();
   init_tables();
   init_variables();
   init_fen(argv[2]);

   printf("fen: %s\n", argv[2]);
   move_t move = iter_dfs(depth, 0);
   printf("> %2i: %2i - %2i [%2i] at depth %i\n", move._.pfrom, move._.from,
      move._.to, move._.pto, depth);

   return 0;
}
