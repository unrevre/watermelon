#include "memory.h"

#include "debug.h"
#include "position.h"
#include "state.h"

#include <stdlib.h>
#include <string.h>

#define BASKETS   4

#define AGEMASK   0x3

#define exact     0x1
#define lower     0x2
#define upper     0x3

ttentry_t ttable[HASHSIZE] __attribute__((aligned(64)));

/*!
 * matching
 * @ helper function to check hash signatures
 */

static inline int64_t matching(ttentry_t* entry, uint32_t hash) {
   return (entry->_.hash ^ entry->_.move.bits >> HASHBITS) == hash >> HASHBITS;
}

void store_hash(transient_t* state, int32_t depth, int32_t alpha, int32_t beta,
                int32_t score, move_t move) {
   int32_t flags = abs(score) > INFLIMIT ? exact : 0;
   flags = flags ? flags : score <= alpha ? upper : 0;
   flags = flags ? flags : score >= beta ? lower : exact;

   int32_t adjust = score < -INFLIMIT ? -state->ply : 0;
   score += score > INFLIMIT ? state->ply : adjust;

   ttentry_t new = {
      ._ = {
         (state->hash ^ move.bits) >> HASHBITS,
         flags,
         depth,
         score,
         trunk.ply & AGEMASK,
         move
      }
   };

   int64_t age_prefer = -1;
   int64_t depth_prefer = state->hash & HASHMASK;
   for (uint32_t t = 0; t != BASKETS; ++t) {
      uint32_t index = (state->hash & HASHMASK) ^ t;
      if (!ttable[index].bits || matching(ttable + index, state->hash)) {
         ttable[index] = new; return; }

      if (ttable[index]._.age != (trunk.ply & AGEMASK)) {
         age_prefer = index; }
      if (ttable[index]._.depth < ttable[depth_prefer]._.depth) {
         depth_prefer = index; }
   }

   int64_t replace = age_prefer >= 0 ? age_prefer : depth_prefer;
   ttable[replace] = new;
}

int32_t probe_hash(transient_t* state, int32_t depth, int32_t* alpha,
                   int32_t* beta, move_t* move) {
   for (uint32_t t = 0; t < BASKETS; ++t) {
      ttentry_t entry = ttable[(state->hash & HASHMASK) ^ t];
      if (matching(&entry, state->hash)) {
         if (!is_valid(state, entry._.move)) { break; }
         *move = entry._.move;

         if (entry._.depth < depth) { break; }
         debug_counter_increment(tthits);

         int32_t score = entry._.score;
         int32_t adjust = score < -INFLIMIT ? state->ply : 0;
         score += score > INFLIMIT ? -state->ply : adjust;

         if (entry._.flags == exact) { return score; }
         if (entry._.flags == lower) {
            *alpha = score > *alpha ? score : *alpha; }
         else {
            *beta = score < *beta ? score : *beta; }

         if (*alpha >= *beta) { return score; }
         break;
      }
   }

   return -INFINITY;
}

ttentry_t entry_for_state(transient_t* state) {
   for (uint32_t t = 0; t != BASKETS; ++t) {
      uint32_t index = (state->hash & HASHMASK) ^ t;
      if (matching(ttable + index, state->hash)
            && ttable[index]._.flags == exact) {
         return ttable[index]; }
   }

   return (ttentry_t){0};
}

move_t move_for_state(transient_t* state) {
   for (uint32_t t = 0; t != BASKETS; ++t) {
      uint32_t index = (state->hash & HASHMASK) ^ t;
      if (matching(ttable + index, state->hash)
            && ttable[index]._.flags == exact) {
         return ttable[index]._.move; }
   }

   return (move_t){0};
}
