#include "search.h"

#include "debug.h"
#include "eval.h"
#include "generate.h"
#include "inlines.h"
#include "magics.h"
#include "memory.h"
#include "position.h"
#include "state.h"
#include "timer.h"

#include <stdlib.h>

#define FDEPTH    3
#define FMARGIN   16

move_t iter_dfs(int32_t depth) {
   reset_search();
   start(search.clock);

   for (int32_t d = 1; d != depth; ++d) {
      tree_root_entry();
      tree_node_entry(-INFINITY, INFINITY);
      int32_t score = negamax(d, -INFINITY, INFINITY, 1);
      tree_node_exit(-INFINITY, INFINITY, score);
      tree_root_exit();

      if (abs(score) >= INFDELAY - d) { break; }
      if (tick(search.clock)) { break; }
   }

   return probe_hash_for_entry()._.move;
}

int32_t negamax(int32_t depth, int32_t alpha, int32_t beta,
                uint32_t principal) {
   ++search.nodes;

   if (!(search.nodes & TIME_RES)) { tick(search.clock); }
   if (search.clock->status) { return 0; }

   alpha = alpha < -LSCORE + state.ply ?  -LSCORE + state.ply : alpha;
   beta =  beta > WSCORE - state.ply ? WSCORE - state.ply : beta;
   if (alpha >= beta) { return alpha; }

   int32_t alpha_parent = alpha;
   move_t store = (move_t){0};

   if (is_repetition()) { return WSCORE - state.ply; }

   int32_t best = -INFSCORE + state.ply;
   if (!principal && (best = probe_hash(depth, &alpha, &beta, &store))
         != -INFSCORE) { return best; }

   if (!depth) { return quiescence(alpha, beta); }

   int32_t stand = eval(state.side);
   if (!principal && depth < FDEPTH && stand - FMARGIN >= beta
         && stand < WSCORE) {
      return stand; }

   if (state.ply > 1 && depth > 4 && !principal && !in_check(state.side)) {
      move_t null = { ._ = { 0x7f, 0x7f, empty, empty } };

      advance(null);
      __builtin_prefetch(&ttable[state.hash & (HASHMASK ^ 0x3)], 1, 3);
      tree_node_entry(alpha, beta);
      int32_t score = -negamax(depth - 3, -beta, -beta + 1, 0);
      tree_node_exit(alpha, beta, score);
      retract(null);

      if (score >= beta) { return score; }
   }

   generator_t engine = { 0, 0, { 0, 0, 0 }, store };

   move_t move;

   while ((move = next(&engine)).bits != 0) {
      advance(move);
      __builtin_prefetch(&ttable[state.hash & (HASHMASK ^ 0x3)], 1, 3);
      tree_node_entry(alpha, beta);
      int32_t score;
      if (best == -INFSCORE + state.ply) {
         score = -negamax(depth - 1, -beta, -alpha, principal);
      } else {
         score = -negamax(depth - 1, -alpha - 1, -alpha, 0);
         if (score > alpha && score < beta)
            score = -negamax(depth - 1, -beta, -alpha, 1);
      }
      tree_node_exit(alpha, beta, score);
      retract(move);

      if (score > best) { store = move; }
      best = score > best ? score : best;
      alpha = score > alpha ? score : alpha;
      if (alpha < beta) { continue; }

      killer_t* killers = &ktable[state.ply];
      switch (engine.state) {
         case 4:
            killers->count++;
            break;
         case 5:
            killers->count--;
            if (killers->count < 1) {
               killers->first = killers->second;
               killers->second = (move_t){0};
               killers->count = 0;
            }
            break;
         case 6:
            killers->second = move;
            killers->count = 0;
            break;
      }

      break;
   }

   if (engine.state > 1) { free(engine.moves.data); }

   if (!search.clock->status) {
      store_hash(depth, alpha_parent, beta, best, store); }

   return best;
}

int32_t quiescence(int32_t alpha, int32_t beta) {
   debug_variable_increment(1, &search.qnodes);

   int32_t stand = eval(state.side);
   if (stand >= beta) { return stand; }
   alpha = stand > alpha ? stand : alpha;

   move_array_t moves = generate_captures(state.side);
   sort_moves(&moves);
   for (int64_t i = 0; i != moves.count; ++i) {
      advance(moves.data[i]);
      tree_node_entry(alpha, beta);
      int32_t score = -quiescence(-beta, -alpha);
      tree_node_exit(alpha, beta, score);
      retract(moves.data[i]);

      alpha = score > alpha ? score : alpha;
      if (alpha >= beta) { break; }
   }

   free(moves.data);

   return alpha;
}
