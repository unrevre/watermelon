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
      tree_node_entry(-INFINITY, INFINITY);
      int32_t score = negamax(d, -INFINITY, INFINITY, 1);
      tree_node_exit(-INFINITY, INFINITY, score);
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

int32_t negamax(uint32_t depth, int32_t alpha, int32_t beta,
                uint32_t principal) {
   debug_variable_increment(1, &nodes);

   alpha = max(alpha, -LSCORE + state.ply);
   beta = min(beta, WSCORE - state.ply);
   if (alpha >= beta) { return alpha; }

   int32_t alpha_parent = alpha;
   move_t store = (move_t){0};

   if (state.step > 4) {
      uint32_t curr = state.step & 0x7;
      uint32_t prev = (state.step - 1) & 0x7;
      if (htable[curr] == htable[curr ^ 0x4]
            && htable[prev] == htable[prev ^ 0x4])
         return WSCORE - state.ply;
   }

   int32_t best = -INFSCORE + state.ply;
   if (!principal && (best = probe_hash(depth, &alpha, &beta, &store))
         != (int32_t)(-INFSCORE + state.ply))
      return best;

   if (!depth) { return quiescence(alpha, beta); }

   if (state.ply > 1 && depth > 4 && !principal && !in_check(state.side)) {
      move_t null = { ._ = { 0x7f, 0x7f, empty, empty } };

      advance(null);
      __builtin_prefetch(&ttable[state.hash & (HASHMASK ^ 0x3)], 1, 3);
      tree_node_entry(alpha, beta);
      int32_t score = -negamax(depth - 3, -beta, -beta + 1, 0);
      tree_node_exit(alpha, beta, score);
      retract(null);

      best = max(best, score);
      alpha = max(alpha, score);

      if (alpha >= beta) { return best; }
   }

   generator_t engine = { 0, 0, {0, 0, 0}, store };

   move_t move;

   while ((move = next(&engine)).bits != 0) {
      advance(move);
      __builtin_prefetch(&ttable[state.hash & (HASHMASK ^ 0x3)], 1, 3);
      tree_node_entry(alpha, beta);
      int32_t score;
      if (best == (int32_t)(-INFSCORE + state.ply)) {
         score = -negamax(depth - 1, -beta, -alpha, principal);
      } else {
         score = -negamax(depth - 1, -alpha - 1, -alpha, 0);
         if (score > alpha && score < beta)
            score = -negamax(depth - 1, -beta, -alpha, 1);
      }
      tree_node_exit(alpha, beta, score);
      retract(move);

      if (score > best) { store = move; }

      best = max(best, score);
      alpha = max(alpha, score);

      if (alpha < beta) { continue; }

      switch (engine.state) {
         case 4:
            ktable[state.ply][0].count++;
            break;
         case 5:
            ktable[state.ply][1].count++;
            if (ktable[state.ply][1].count > ktable[state.ply][0].count) {
               ktable[state.ply][0] = ktable[state.ply][1];
               ktable[state.ply][1] = (killer_t){{0}, 0};
            }
            break;
         case 6:
            if (!ktable[state.ply][0].move.bits) {
               ktable[state.ply][0].move = move;
            } else {
               if (!ktable[state.ply][1].count) {
                  ktable[state.ply][1].move = move;
                  ktable[state.ply][1].count++;
               } else {
                  ktable[state.ply][1].count--;
               }
            }
            break;
      }

      break;
   }

   if (engine.state > 1) { free(engine.moves.data); }
   store_hash(depth, alpha_parent, beta, best, store);

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
      tree_node_entry(alpha, beta);
      int32_t score = -quiescence(-beta, -alpha);
      tree_node_exit(alpha, beta, score);
      retract(moves.data[i]);

      alpha = max(alpha, score);
      if (alpha >= beta) { break; }
   }

   free(moves.data);

   return alpha;
}
