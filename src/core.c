#include "core.h"

#include "debug.h"
#include "generate.h"
#include "memory.h"
#include "search.h"
#include "state.h"

#include <stdlib.h>
#include <string.h>

#define MAX_THREADS  1

settings_t settings;

search_t search;
worker_t workers[MAX_THREADS];

void initialise(const char* fen) {
   init_hashes();

   set_state(fen);

   memset(ttable, 0, HASHSIZE * sizeof(ttentry_t));
   memset(ktable, 0, PLYLIMIT * sizeof(killer_t));

   search.clock = malloc(sizeof(wmclock_t));
   search.clock->status = 1;

   settings.threads = 1;
   settings.limit = -1.;
}

void terminate(void) {
   free(search.clock);
}

void set_limit(double limit) {
   settings.limit = limit;
}

void set_threads(int64_t threads) {
   settings.threads = threads < MAX_THREADS ? threads : MAX_THREADS;
}

void* smp_worker(void* worker __attribute__((unused))) {
   transient_t* state = malloc(sizeof(transient_t));

   memcpy(state, &trunk, sizeof(transient_t));
   state->ply = 0;

   iter_dfs(state);

   free(state);
   return NULL;
}

void smp_search(int32_t depth) {
   search.clock->status = 0;
   search.clock->limit = settings.limit;

   search.target = depth;
   search.depth = 0;
   search.nodes = 0;
   search.qnodes = 0;
   search.tthits = 0;

   transient_t* state = malloc(sizeof(transient_t));
   tree_debug_state(state);

   memcpy(state, &trunk, sizeof(transient_t));
   state->ply = 0;

   start(search.clock);

   for (int64_t i = 1; i != settings.threads; ++i)
      pthread_create(&workers[i].thread, NULL, smp_worker, &workers[i]);

   iter_dfs(state);

   for (int64_t i = 1; i != settings.threads; ++i)
      pthread_join(workers[i].thread, NULL);

   tree_debug_state(&trunk);
   free(state);
}

int32_t smp_depth(void) {
   return ++search.depth % search.target;
}
