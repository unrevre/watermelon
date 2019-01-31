#include "search.h"

#include "debug.h"
#include "eval.h"
#include "generate.h"
#include "inlines.h"
#include "magics.h"
#include "memory.h"
#include "state.h"

#include <stdlib.h>

move_t iter_dfs(uint32_t depth) {
   debug_variable_reset(3, &nodes, &qnodes, &tthits);

   ++age;

   for (uint32_t d = 1; d != depth; ++d) {
      tree_root_entry();
      tree_node_entry(-INFINITY, INFINITY, 0);
      int32_t score = negamax(d, -INFINITY, INFINITY);
      tree_node_exit(-INFINITY, INFINITY, score, 0);
      tree_root_exit();

      if (int32t_abs(score) >= INFDELAY - d) { break; }
   }

   move_t move = {0};
   for (uint32_t t = 0; t != BASKETS; ++t) {
      uint32_t index = (state.hash & HASHMASK) ^ t;
      if (ttable[index]._.hash == state.hash >> HASHBITS
            && ttable[index]._.flags == FEXACT) {
         move = ttable[index]._.move;
         break;
      }
   }

   return move;
}

int32_t negamax(uint32_t depth, int32_t alpha, int32_t beta) {
   debug_variable_increment(1, &nodes);

   alpha = max(alpha, -LSCORE + state.ply);
   beta = min(beta, WSCORE - state.ply);

   int32_t alpha_parent = alpha;
   move_t move_store = (move_t){0};

   if (state.step > 3) {
      uint32_t curr = state.step & 0x7;
      if (htable[curr] == htable[curr ^ 0x4]) {
         uint32_t prev = (state.step - 1) & 0x7;
         if (state.step > 4 && htable[prev] == htable[prev ^ 0x4])
            return WSCORE - state.ply;
         else
            goto search_quiescence;
      }
   }

   int32_t best = probe_hash(depth, &alpha, &beta, &move_store);
   if (best != (int32_t)(-INFSCORE + state.ply)) { return best; }

search_quiescence:
   if (!depth) {
      tree_node_entry(alpha, beta, 1);
      int32_t score = quiescence(alpha, beta);
      tree_node_exit(alpha, beta, score, 1);
      return score;
   }

   if (move_store.bits) {
      advance(move_store);
      __builtin_prefetch(&ttable[state.hash & (HASHMASK ^ 0x3)], 1, 3);
      tree_node_entry(alpha, beta, 0);
      int32_t score = -negamax(depth - 1, -beta, -alpha);
      tree_node_exit(alpha, beta, score, 0);
      retract(move_store);

      best = max(best, score);
      alpha = max(alpha, score);

      if (alpha >= beta) { goto search_save; }
   }

   if (state.ply > 1 && depth > 4 && !in_check(state.side)) {
      move_t null = { ._ = { 0x7f, 0x7f, empty, empty } };
      advance(null);
      tree_node_entry(alpha, beta, 0);
      int32_t score = -negamax(depth - 3, -beta, -alpha);
      tree_node_exit(alpha, beta, score, 0);
      retract(null);

      if (score > best) { move_store = (move_t){0}; }

      best = max(best, score);
      alpha = max(alpha, score);

      if (alpha >= beta) { goto search_save; }
   }

   move_array_t moves = sort_moves(generate_pseudolegal(state.side));

   for (uint32_t i = 0; i != moves.quiet; ++i) {
      advance(moves.data[i]);
      __builtin_prefetch(&ttable[state.hash & (HASHMASK ^ 0x3)], 1, 3);
      tree_node_entry(alpha, beta, 0);
      int32_t score = -negamax(depth - 1, -beta, -alpha);
      tree_node_exit(alpha, beta, score, 0);
      retract(moves.data[i]);

      if (score > best) { move_store = moves.data[i]; }

      best = max(best, score);
      alpha = max(alpha, score);

      if (alpha >= beta) { goto search_free; }
   }

   move_t move_killer;
   for (uint32_t k = 0; k < 2; ++k) {
      move_killer = ktable[state.ply][k].move;
      if (move_killer.bits && is_legal(move_killer, state.side)) {
         advance(move_killer);
         __builtin_prefetch(&ttable[state.hash & (HASHMASK ^ 0x3)], 1, 3);
         tree_node_entry(alpha, beta, 0);
         int32_t score = -negamax(depth - 1, -beta, -alpha);
         tree_node_exit(alpha, beta, score, 0);
         retract(move_killer);

         if (score > best) { move_store = move_killer; }

         best = max(best, score);
         alpha = max(alpha, score);

         if (alpha >= beta) {
            ktable[state.ply][k].count++;
            if (ktable[state.ply][1].count > ktable[state.ply][0].count) {
               ktable[state.ply][0] = ktable[state.ply][1];
               ktable[state.ply][1] = (killer_t){{0}, 0};
            }

            goto search_free;
         }
      }
   }

   for (uint32_t i = moves.quiet; i != moves.count; ++i) {
      advance(moves.data[i]);
      __builtin_prefetch(&ttable[state.hash & (HASHMASK ^ 0x3)], 1, 3);
      tree_node_entry(alpha, beta, 0);
      int32_t score = -negamax(depth - 1, -beta, -alpha);
      tree_node_exit(alpha, beta, score, 0);
      retract(moves.data[i]);

      if (score > best) { move_store = moves.data[i]; }

      best = max(best, score);
      alpha = max(alpha, score);

      if (alpha >= beta) {
         if (!ktable[state.ply][0].move.bits) {
            ktable[state.ply][0].move = move_store;
         } else {
            if (!ktable[state.ply][1].count) {
               ktable[state.ply][1].move = move_store;
               ktable[state.ply][1].count++;
            } else {
               ktable[state.ply][1].count--;
            }
         }

         break;
      }
   }

search_free:
   free(moves.data);

search_save:
   store_hash(depth, alpha_parent, beta, best, move_store);

   return best;
}

int32_t quiescence(int32_t alpha, int32_t beta) {
   debug_variable_increment(1, &qnodes);

   int32_t stand = eval(state.side);
   if (stand >= beta) { return stand; }

   alpha = max(alpha, stand);

   move_array_t moves = sort_moves(generate_captures(state.side));
   for (uint32_t i = 0; i != moves.count; ++i) {
      advance(moves.data[i]);
      tree_node_entry(alpha, beta, 1);
      int32_t score = -quiescence(-beta, -alpha);
      tree_node_exit(alpha, beta, score, 1);
      retract(moves.data[i]);

      alpha = max(alpha, score);
      if (alpha >= beta) { break; }
   }

   free(moves.data);

   return alpha;
}
