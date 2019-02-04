#include "../../src/debug.h"
#include "../../src/magics.h"
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

   char* buffer = calloc(201, sizeof(char));
   char** buffers = calloc(PLYLIMIT, sizeof(char*));

   info_move(buffer, move);
   printf("%s at depth %i\n", buffer, depth);

   trace_principal_variation(buffers);
   for (uint32_t i = 0; i < PLYLIMIT && buffers[i]; ++i)
      printf("%s", buffers[i]);
   printf("\n");

   for (uint32_t i = 0; i < PLYLIMIT && buffers[i]; ++i)
      free(buffers[i]);

   free(buffer);
   free(buffers);

   return 0;
}
