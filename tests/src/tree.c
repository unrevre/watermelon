#include "core.h"
#include "debug.h"
#include "search.h"
#include "state.h"
#include "structs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char* argv[]) {
   if (argc != 2 && argc != 3) {
      printf("usage: %s [depth] [fen]\n", argv[0]);
      return 1;
   }

   initialise(argc == 3 ? argv[2] : 0);

   search.status = 1;
   search.ref = time(NULL);
   search.target = atoi(argv[1]);
   search.depth = 1;
   search.count = 1;

   struct debug_t* info = malloc(sizeof(struct debug_t));
   init_debug(info);

   info_fen(info->buffer);
   printf("%s\n", info->buffer);

   struct transient_t* state = malloc(sizeof(struct transient_t));
   memcpy(state, &trunk, sizeof(struct transient_t));

   tree_debug_state(state);

   iter_dfs(state);

   free(state);
   free(info);

   terminate();

   return 0;
}
