#include "search.h"

#include "eval.h"
#include "generate.h"
#include "inlines.h"
#include "magics.h"
#include "state.h"

#include <stdlib.h>

#ifdef DEBUG
uint32_t nodes;
uint32_t qnodes;

uint32_t tthits;
#endif

#ifdef TREE
#include "debug.h"
#endif

move_t iter_dfs(uint32_t depth, uint32_t side) {
#ifdef DEBUG
   nodes = 0;
   qnodes = 0;

   tthits = 0;
#endif

   ++age;

   for (uint32_t d = 1; d != depth; ++d) {
#ifdef TREE
      tree_root_entry();
#endif
      int32_t score = negamax(d, 1, -INFINITY, INFINITY, side);
#ifdef TREE
      tree_node_exit(0, -INFINITY, INFINITY, score, side);
      tree_root_exit();
#endif

      if (int32t_abs(score) >= INFDELAY - d) { break; }
   }

   move_t move = {0};
   for (uint32_t t = 0; t != BASKETS; ++t) {
      uint32_t index = (hash_state & HASHMASK) ^ t;
      if (ttable[index]._.hash == hash_state >> HASHBITS
            && ttable[index]._.flags == FEXACT) {
         move = ttable[index]._.move;
         break;
      }
   }

   return move;
}

int32_t negamax(uint32_t depth, uint32_t ply, int32_t alpha, int32_t beta,
                uint32_t side) {
#ifdef DEBUG
   ++nodes;
#endif

   alpha = max(alpha, -LSCORE + ply);
   beta = min(beta, WSCORE - ply);

#ifdef TREE
   tree_node_entry(ply, alpha, beta, side);
#endif

   int32_t alpha_parent = alpha;

   move_t move_store = (move_t){0};

   if (step > 3) {
      uint32_t curr = step & 0x7;
      if (htable[curr] == htable[curr ^ 0x4]) {
         uint32_t prev = (step - 1) & 0x7;
         if (step > 4 && htable[prev] == htable[prev ^ 0x4])
            return WSCORE - ply;
         else
            goto search_quiescence;
      }
   }

   for (uint32_t t = 0; t < BASKETS; ++t) {
      ttentry_t entry = ttable[(hash_state & HASHMASK) ^ t];
      if (entry._.hash == hash_state >> HASHBITS && entry._.move.bits) {
         if (!is_legal(entry._.move, side)) { continue; }

         move_store = entry._.move;

         if (entry._.depth < depth) { continue; }

#ifdef DEBUG
         ++tthits;
#endif

         int32_t score = entry._.score;
         score = (score > WSCORE - PLYLIMIT) ? score - ply :
            (score < -LSCORE + PLYLIMIT) ? score + ply : score;

         switch (entry._.flags) {
            case FEXACT:
               return score;
            case FLOWER:
               alpha = max(alpha, score);
               break;
            case FUPPER:
               beta = min(beta, score);
               break;
         }

         if (alpha >= beta) { return score; }

         break;
      }
   }

search_quiescence:
   if (!depth) {
      int32_t score = quiescence(ply, alpha, beta, side);
#ifdef TREE
      tree_node_exit(ply, alpha, beta, score, side);
#endif
      return score;
   }

   int32_t best = -INFSCORE + ply;

   if (move_store.bits) {
      advance(move_store);

      int32_t score = -negamax(depth - 1, ply + 1, -beta, -alpha, o(side));
#ifdef TREE
      tree_node_exit(ply, alpha, beta, score, side);
#endif

      retract(move_store);

      best = max(best, score);
      alpha = max(alpha, score);

      if (alpha >= beta) { goto search_save; }
   }

   if (ply > 1 && depth > 4 && !in_check(side)) {
      hash_state ^= MVHASH;
      int32_t score = -negamax(depth - 3, ply + 1, -beta, -alpha, o(side));
#ifdef TREE
      tree_node_exit(ply, alpha, beta, score, side);
#endif
      hash_state ^= MVHASH;

      if (score > best) { move_store = (move_t){0}; }

      best = max(best, score);
      alpha = max(alpha, score);

      if (alpha >= beta) { goto search_save; }
   }

   move_array_t moves = sort_moves(generate_pseudolegal(side));

   for (uint32_t i = 0; i != moves.quiet; ++i) {
      advance(moves.data[i]);

      int32_t score = -negamax(depth - 1, ply + 1, -beta, -alpha, o(side));
#ifdef TREE
      tree_node_exit(ply, alpha, beta, score, side);
#endif

      retract(moves.data[i]);

      if (score > best) { move_store = moves.data[i]; }

      best = max(best, score);
      alpha = max(alpha, score);

      if (alpha >= beta) { goto search_free; }
   }

   move_t move_killer;
   for (uint32_t k = 0; k < 2; ++k) {
      move_killer = ktable[ply][k].move;
      if (move_killer.bits && is_legal(move_killer, side)) {
         advance(move_killer);
         int32_t score = -negamax(depth - 1, ply + 1, -beta, -alpha, o(side));
#ifdef TREE
         tree_node_exit(ply, alpha, beta, score, side);
#endif
         retract(move_killer);

         if (score > best) { move_store = move_killer; }

         best = max(best, score);
         alpha = max(alpha, score);

         if (alpha >= beta) {
            ktable[ply][k].count++;
            if (ktable[ply][1].count > ktable[ply][0].count) {
               ktable[ply][0] = ktable[ply][1];
               ktable[ply][1] = (killer_t){{0}, 0};
            }

            goto search_free;
         }
      }
   }

   for (uint32_t i = moves.quiet; i != moves.count; ++i) {
      advance(moves.data[i]);

      int32_t score = -negamax(depth - 1, ply + 1, -beta, -alpha, o(side));
#ifdef TREE
      tree_node_exit(ply, alpha, beta, score, side);
#endif

      retract(moves.data[i]);

      if (score > best) { move_store = moves.data[i]; }

      best = max(best, score);
      alpha = max(alpha, score);

      if (alpha >= beta) {
         if (!ktable[ply][0].move.bits) {
            ktable[ply][0].move = move_store;
         } else {
            if (!ktable[ply][1].count) {
               ktable[ply][1].move = move_store;
               ktable[ply][1].count++;
            } else {
               ktable[ply][1].count--;
            }
         }

         break;
      }
   }

search_free:
   free(moves.data);

search_save:
   store_hash(depth, ply, alpha_parent, beta, best, move_store);

   return best;
}

int32_t quiescence(uint32_t ply, int32_t alpha, int32_t beta, uint32_t side) {
#ifdef DEBUG
   ++qnodes;
#endif

   int32_t stand = eval(side);
#ifdef TREE
   tree_node_entry(ply + 1, alpha, beta, side);
#endif
   if (stand >= beta) { return stand; }

   alpha = max(alpha, stand);

   move_array_t moves = sort_moves(generate_captures(side));
   for (uint32_t i = 0; i != moves.count; ++i) {
      advance(moves.data[i]);

      int32_t score = -quiescence(ply + 1, -beta, -alpha, o(side));
#ifdef TREE
      tree_node_exit(ply + 1, alpha, beta, score, side);
#endif

      retract(moves.data[i]);

      alpha = max(alpha, score);
      if (alpha >= beta) { break; }
   }

   free(moves.data);

   return alpha;
}

void store_hash(uint32_t depth, uint32_t ply, int32_t alpha, int32_t beta,
                int32_t score, move_t move_hashed) {
   uint32_t index = hash_state & HASHMASK;

   uint8_t flags = (int32t_abs(score) > WSCORE - PLYLIMIT) ? FEXACT :
      (score <= alpha) ? FUPPER : (score >= beta) ? FLOWER : FEXACT;

   score = (score > WSCORE - PLYLIMIT) ? score + ply :
      (score < -LSCORE + PLYLIMIT) ? score - ply : score;

   uint32_t replace = HASHSIZE;
   for (uint32_t t = 0; t != BASKETS; ++t) {
      uint32_t entry = index ^ t;
      if (!ttable[entry].bits) {
         replace = entry;
         break;
      } else if (ttable[entry]._.hash == hash_state >> HASHBITS) {
         replace = entry;
         break;
      } else if (ttable[entry]._.age != age) {
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

   ttable[replace] = (ttentry_t) {
      ._ = { hash_state >> HASHBITS, depth, flags, score, age, move_hashed }
   };
}
