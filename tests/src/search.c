#include "../../src/debug.h"
#include "../../src/memory.h"
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

   debug_t* info = malloc(sizeof(debug_t));
   init_debug(info);

   init_state(argv[2]);
   printf("%s\n", info_fen(info));

   int32_t depth = atoi(argv[1]);
   move_t move = iter_dfs(depth);
   printf("%s\n\n", info_move(info, move));

   char** pv = info_principal_variation(info);
   for (int64_t i = 0; i < PLYLIMIT && pv[i][0]; ++i)
      printf("%s", pv[i]);
   printf("\n");

   free_debug(info);

   return 0;
}
