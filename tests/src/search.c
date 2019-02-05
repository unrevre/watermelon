#include "../../src/debug.h"
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

   init_state(argv[2]);

   debug_t info;
   init_debug(&info);

   printf("%s\n", info_fen(&info));

   uint32_t depth = atoi(argv[1]);

   move_t move = iter_dfs(depth);
   printf("%s at depth %i\n", info_move(&info, move), depth);
   printf("%s\n", info_principal_variation(&info));

   free_debug(&info);

   return 0;
}
