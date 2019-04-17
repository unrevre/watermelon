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

move_t history[STEPLIMIT];
uint32_t htable[STEPLIMIT];
ttentry_t ttable[HASHSIZE] __attribute__((aligned(64)));
killer_t ktable[PLYLIMIT] __attribute__((aligned(64)));

void reset_tables(void) {
   memset(history, 0, STEPLIMIT * sizeof(move_t));
   memset(htable, 0, STEPLIMIT * sizeof(uint32_t));
   memset(ttable, 0, HASHSIZE * sizeof(ttentry_t));
   memset(ktable, 0, PLYLIMIT * sizeof(killer_t));
}

void store_hash(transient_t* state, int32_t depth, int32_t alpha, int32_t beta,
                int32_t score, move_t move) {
   int32_t flags = abs(score) > INFLIMIT ? exact : 0;
   flags = flags ? flags : score <= alpha ? upper : 0;
   flags = flags ? flags : score >= beta ? lower : exact;

   int32_t adjust = score < -INFLIMIT ? -state->ply : 0;
   score += score > INFLIMIT ? state->ply : adjust;

   ttentry_t new = { ._ = {
      state->hash >> HASHBITS, flags, depth, score, age, move } };

   int64_t age_prefer = -1;
   int64_t depth_prefer = state->hash & HASHMASK;
   for (uint32_t t = 0; t != BASKETS; ++t) {
      uint32_t index = (state->hash & HASHMASK) ^ t;
      if (!ttable[index].bits
            || ttable[index]._.hash == state->hash >> HASHBITS) {
         ttable[index] = new; return; }

      if (ttable[index]._.age != (state->step & AGEMASK)) {
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
      if (entry._.hash == state->hash >> HASHBITS) {
         if (!is_valid(entry._.move, state->side)) { break; }
         *move = entry._.move;

         if (entry._.depth < depth) { break; }
         debug_variable_increment(1, &search.tthits);

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

ttentry_t probe_hash_for_entry(transient_t* state) {
   for (uint32_t t = 0; t != BASKETS; ++t) {
      uint32_t index = (state->hash & HASHMASK) ^ t;
      if (ttable[index]._.hash == state->hash >> HASHBITS
            && ttable[index]._.flags == exact) {
         return ttable[index]; }
   }

   return (ttentry_t){0};
}

void advance_history(move_t move) {
   int32_t step = trunk.step;
   move_t future = history[step];
   if (future.bits && move.bits != future.bits)
      while (history[++step].bits)
         history[step] = (move_t){0};

   history[trunk.step] = move;
}

void undo_history(void) {
   if (trunk.step) { retract_game(history[trunk.step - 1]); }
}

void redo_history(void) {
   if (history[trunk.step].bits) {
      advance_history(history[trunk.step]);
      advance_game(history[trunk.step]);
   }
}
