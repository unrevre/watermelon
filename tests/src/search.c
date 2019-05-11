#include "../../src/debug.h"
#include "../../src/memory.h"
#include "../../src/search.h"
#include "../../src/state.h"

#define __STDC_FORMAT_MACROS
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
   if (argc != 3) {
      printf("usage: %s [depth] [fen]\n", argv[0]);
      return 1;
   }

   initialise(argv[2]);

   debug_t* info = malloc(sizeof(debug_t));
   init_debug(info);

   printf("%s\n", info_fen(info));
   move_t move = iter_dfs(atoi(argv[1]));
   printf("%s\n\n", info_move(info, move));

   char** buffers = info_principal_variation(info);
   for (int64_t i = 0; i < PLYLIMIT && buffers[i][0]; ++i)
      printf("%s", buffers[i]);
   printf("\n");

   free_debug(info);

   terminate();

   return 0;
}
