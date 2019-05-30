#include "core.h"

#include "debug.h"
#include "generate.h"
#include "memory.h"
#include "search.h"
#include "state.h"

#include <stdlib.h>
#include <string.h>

#define MAX_THREADS  4

search_t search;
worker_t workers[MAX_THREADS];

void initialise(const char* fen) {
   init_hashes();

   set_state(fen);

   memset(ttable, 0, HASHSIZE * sizeof(ttentry_t));
   memset(ktable, 0, PLYLIMIT * sizeof(killer_t));

   search.status = 0;
   search.threads = 1;
   search.limit = -1.;
}

void terminate(void) {
   ;
}

void set_limit(double limit) {
   search.limit = limit;
}

void set_threads(int64_t threads) {
   search.threads = threads < MAX_THREADS ? threads : MAX_THREADS;
}

/*!
 * smp_worker
 * @ worker function for multithreaded search
 */

void* smp_worker(void* worker __attribute__((unused))) {
   transient_t* state = malloc(sizeof(transient_t));

   memcpy(state, &trunk, sizeof(transient_t));
   state->ply = 0;

   iter_dfs(state);

   free(state);
   return NULL;
}

void smp_search(int32_t depth) {
   debug_counter_reset();

   search.status = 1;
   search.ref = time(NULL);
   search.target = depth;
   search.depth = 1;
   search.count = search.threads;

   pthread_mutex_init(&search.lock, NULL);

   for (int64_t i = 0; i != search.threads; ++i)
      pthread_create(&workers[i].thread, NULL, smp_worker, &workers[i]);

   struct timespec interval;
   interval.tv_sec = 0;
   interval.tv_nsec = 40000000L;

   while (search.status) {
      pthread_mutex_lock(&search.lock);
      if (!search.count || (search.limit > 0 && difftime(
            time(NULL), search.ref) > search.limit))
         search.status = 0;
      pthread_mutex_unlock(&search.lock);

      nanosleep(&interval, NULL);
   }

   for (int64_t i = 0; i != search.threads; ++i)
      pthread_join(workers[i].thread, NULL);

   pthread_mutex_destroy(&search.lock);
}

int32_t smp_depth(void) {
   pthread_mutex_lock(&search.lock);
   int32_t depth = search.depth;
   search.depth = search.depth ? (search.depth + 1) % search.target : 0;
   pthread_mutex_unlock(&search.lock);

   return depth;
}
