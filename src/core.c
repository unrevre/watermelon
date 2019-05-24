#include "core.h"

#include "debug.h"
#include "search.h"
#include "state.h"

#include <stdlib.h>
#include <string.h>

search_t search;

void smp_search(int32_t depth) {
   search.clock->status = 0;

   search.nodes = 0;
   search.qnodes = 0;
   search.tthits = 0;

   transient_t* state = malloc(sizeof(transient_t));
   tree_debug_state(state);

   memcpy(state, &trunk, sizeof(transient_t));
   state->ply = 0;

   start(search.clock);

   iter_dfs(state, depth);

   tree_debug_state(&trunk);
   free(state);
}
