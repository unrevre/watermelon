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
   uint8_t flags = (abs(score) > WSCORE - PLYLIMIT) ? FEXACT :
      (score <= alpha) ? FUPPER : (score >= beta) ? FLOWER : FEXACT;

   score = (score > WSCORE - PLYLIMIT) ? score + state.ply :
      (score < -LSCORE + PLYLIMIT) ? score - state.ply : score;

   uint32_t replace = HASHSIZE;
   uint32_t index = state.hash & HASHMASK;
   for (uint32_t t = 0; t != BASKETS; ++t) {
      uint32_t entry = index ^ t;
      if (!ttable[entry].bits) {
         replace = entry;
         break;
      } else if (ttable[entry]._.hash == state.hash >> HASHBITS) {
         replace = entry;
         break;
      } else if (ttable[entry]._.age != (state.step & 0x3)) {
         replace = entry;
      }
   }

   if (replace == HASHSIZE) {
      uint32_t min_depth = PLYLIMIT;

      replace = index;
      for (uint32_t t = 0; t != BASKETS; ++t) {
         uint32_t entry = index ^ t;
         if (ttable[entry]._.depth < min_depth) {
            min_depth = ttable[entry]._.depth;
            replace = entry;
         }
      }
   }

   ttable[replace] = (ttentry_t) { ._ = {
      state.hash >> HASHBITS, flags, depth, score, state.step, move } };
}

int32_t probe_hash(int32_t depth, int32_t* alpha, int32_t* beta,
                   move_t* move) {
   for (uint32_t t = 0; t < BASKETS; ++t) {
      ttentry_t entry = ttable[(state.hash & HASHMASK) ^ t];
      if (entry._.hash == state.hash >> HASHBITS) {
         if (!is_valid(entry._.move, state.side)) { break; }
         *move = entry._.move;

         if (entry._.depth < depth) { continue; }
         debug_variable_increment(1, &tthits);

         int32_t score = entry._.score;
         score = (score > WSCORE - PLYLIMIT) ? score - state.ply :
            (score < -LSCORE + PLYLIMIT) ? score + state.ply : score;

         switch (entry._.flags) {
            case FEXACT:
               return score;
            case FLOWER:
               *alpha = max(*alpha, score);
               break;
            case FUPPER:
               *beta = min(*beta, score);
               break;
         }

         if (*alpha >= *beta) { return score; }
         break;
      }
   }

   return -INFSCORE + state.ply;
}

ttentry_t probe_hash_for_entry() {
   ttentry_t entry = {0};
   for (uint32_t t = 0; t != BASKETS; ++t) {
      uint32_t index = (state.hash & HASHMASK) ^ t;
      if (ttable[index]._.hash == state.hash >> HASHBITS
            && ttable[index]._.flags == FEXACT) {
         entry = ttable[index];
         break;
      }
   }

   return entry;
}
