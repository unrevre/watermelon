#include "memory.h"

#include "debug.h"
#include "generate.h"
#include "inlines.h"
#include "state.h"

#include <stdlib.h>
#include <string.h>

move_t history[STEPLIMIT];
uint32_t htable[STEPLIMIT];
ttentry_t ttable[HASHSIZE] __attribute__((aligned(64)));
killer_t ktable[PLYLIMIT][2] __attribute__((aligned(64)));

void init_tables(void) {
   memset(htable, 0, 8 * sizeof(uint32_t));
   memset(ttable, 0, HASHSIZE * sizeof(ttentry_t));
   memset(ktable, 0, PLYLIMIT * 2 * sizeof(killer_t));
}

void store_hash(int32_t depth, int32_t alpha, int32_t beta, int32_t score,
                move_t move) {
   int32_t flags = abs(score) > WSCORE - PLYLIMIT ? FEXACT : 0;
   flags = flags ? flags : score <= alpha ? FUPPER : 0;
   flags = flags ? flags : score >= beta ? FLOWER : FEXACT;

   int32_t adjust = score < -LSCORE + PLYLIMIT ? -state.ply : 0;
   score += score > WSCORE - PLYLIMIT ? state.ply : adjust;

   ttentry_t new = { ._ = {
      state.hash >> HASHBITS, flags, depth, score, state.step, move } };

   int64_t age_prefer = -1;
   int64_t depth_prefer = state.hash & HASHMASK;
   for (uint32_t t = 0; t != BASKETS; ++t) {
      uint32_t index = (state.hash & HASHMASK) ^ t;
      if (!ttable[index].bits
            || ttable[index]._.hash == state.hash >> HASHBITS) {
         ttable[index] = new; return; }

      if (ttable[index]._.age != (state.step & 0x3)) {
         age_prefer = index; }
      if (ttable[index]._.depth < ttable[depth_prefer]._.depth) {
         depth_prefer = index; }
   }

   int64_t replace = age_prefer >= 0 ? age_prefer : depth_prefer;
   ttable[replace] = new;
}

int32_t probe_hash(int32_t depth, int32_t* alpha, int32_t* beta,
                   move_t* move) {
   for (uint32_t t = 0; t < BASKETS; ++t) {
      ttentry_t entry = ttable[(state.hash & HASHMASK) ^ t];
      if (entry._.hash == state.hash >> HASHBITS) {
         if (!is_valid(entry._.move, state.side)) { break; }
         *move = entry._.move;

         if (entry._.depth < depth) { break; }
         debug_variable_increment(1, &tthits);

         int32_t score = entry._.score;
         int32_t adjust = score < -LSCORE + PLYLIMIT ? state.ply : 0;
         score += score > WSCORE - PLYLIMIT ? -state.ply : adjust;

         if (entry._.flags == FEXACT) { return score; }
         if (entry._.flags == FLOWER) {
            *alpha = max(*alpha, score); }
         else {
            *beta = min(*beta, score); }

         if (*alpha >= *beta) { return score; }
         break;
      }
   }

   return -INFSCORE + state.ply;
}

ttentry_t probe_hash_for_entry() {
   for (uint32_t t = 0; t != BASKETS; ++t) {
      uint32_t index = (state.hash & HASHMASK) ^ t;
      if (ttable[index]._.hash == state.hash >> HASHBITS
            && ttable[index]._.flags == FEXACT) {
         return ttable[index]; }
   }

   return (ttentry_t){0};
}
