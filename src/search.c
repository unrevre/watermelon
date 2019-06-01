#include "search.h"

#include "core.h"
#include "debug.h"
#include "eval.h"
#include "generate.h"
#include "inlines.h"
#include "magics.h"
#include "memory.h"
#include "position.h"
#include "state.h"

#include <setjmp.h>
#include <stdlib.h>

#define FDEPTH    3
#define FMARGIN   16
#define QMARGIN   4

#define TIMEOUT   1

#ifdef __APPLE__
#define setjmp    _setjmp
#define longjmp   _longjmp
#endif

void iter_dfs(transient_t* state) {
   int32_t depth;
   while ((depth = smp_depth())) {
      if (setjmp(state->env)) { break; }

      int32_t score = 0;
      int32_t delta = 2;

      for (;;) {
         int32_t alpha = score - delta;
         int32_t beta = score + delta;

         tree_root_entry();
         tree_node_entry(alpha, beta);
         score = negamax(depth, state, alpha, beta, 1);
         tree_node_exit(alpha, beta, score);
         tree_root_exit();

         if (score > alpha && score < beta) { break; }

         delta = delta << 2;
      }

      if (abs(score) >= INFINITY - depth) { break; }
   }

   atomaddl(&search.count, -1);
}

int32_t negamax(int32_t depth, transient_t* state, int32_t alpha, int32_t beta,
                uint32_t principal) {
   debug_counter_increment(nodes);

   if (!search.status) { longjmp(state->env, TIMEOUT); }

   alpha = alpha < INFMINUS + state->ply ? INFMINUS + state->ply : alpha;
   beta = beta > INFPLUS - state->ply ? INFPLUS - state->ply : beta;
   if (alpha >= beta) { return alpha; }

   if (is_repetition(state)) { return DRAW; }

   int32_t alpha_parent = alpha;
   move_t store = (move_t){0};

   int32_t hash_score = probe_hash(state, depth, &alpha, &beta, &store);
   if (!principal && hash_score != -INFINITY) {
      return hash_score; }

   if (!depth && in_check(state, state->side)) { ++depth; }
   if (!depth) { return quiescence(state, alpha, beta); }

   int32_t stand = eval(state, state->side);
   if (!principal && depth < FDEPTH && stand - FMARGIN >= beta
         && stand < INFLIMIT && !in_check(state, state->side)) {
      return stand; }

   if (!principal && state->ply > 1 && depth > 4
         && !in_check(state, state->side)) {
      move_t null = { ._ = { 0x7f, 0x7f, empty, empty } };

      advance(null, state);
      __builtin_prefetch(&ttable[state->hash & (HASHMASK ^ 0x3)], 1, 3);
      tree_node_entry(alpha, beta);
      int32_t score = -negamax(depth - 3, state, -beta, -beta + 1, 0);
      tree_node_exit(alpha, beta, score);
      retract(null, state);

      if (score >= beta) { return score; }
   }

   int32_t base = INFMINUS + state->ply;
   generator_t engine = { 0, 0, { 0, 0, 0 }, store };

   int32_t best = base;
   move_t move;

   int32_t reduced = depth;

   while ((move = next(&engine, state)).bits != 0) {
      advance(move, state);
      __builtin_prefetch(&ttable[state->hash & (HASHMASK ^ 0x3)], 1, 3);
      tree_node_entry(alpha, beta);

      if (depth > 3 && engine.state > 3
            && ((reduced == depth && engine.index > 3)
            || (reduced == depth - 1 && engine.index > 24))
            && !in_check(state, state->side)) {
         --reduced; }

      int32_t score;
      if (best == base) {
         score = -negamax(depth - 1, state, -beta, -alpha, principal);
      } else {
         score = -negamax(reduced - 1, state, -alpha - 1, -alpha, 0);

         if (score > alpha && reduced != depth) {
            score = -negamax(depth - 1, state, -alpha - 1, -alpha, 0); }

         if (score > alpha && score < beta) {
            score = -negamax(depth - 1, state, -beta, -alpha, 1); }
      }

      tree_node_exit(alpha, beta, score);
      retract(move, state);

      if (score > best) { store = move; }
      best = score > best ? score : best;
      alpha = score > alpha ? score : alpha;
      if (alpha < beta) { continue; }

      killer_t* killers = &ktable[state->ply];
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

   store_hash(state, depth, alpha_parent, beta, best, store);

   return best;
}

int32_t quiescence(transient_t* state, int32_t alpha, int32_t beta) {
   debug_counter_increment(qnodes);

   int32_t stand = eval(state, state->side);
   if (stand >= beta) { return stand; }
   alpha = stand > alpha ? stand : alpha;

   move_array_t moves = generate_captures(state);
   sort_moves(&moves);
   for (int64_t i = 0; i != moves.count; ++i) {
      if (stand + gain(moves.data[i]) + QMARGIN < alpha) {
         continue; }

      advance(moves.data[i], state);
      tree_node_entry(alpha, beta);
      int32_t score = -quiescence(state, -beta, -alpha);
      tree_node_exit(alpha, beta, score);
      retract(moves.data[i], state);

      alpha = score > alpha ? score : alpha;
      if (alpha >= beta) { break; }
   }

   free(moves.data);

   return alpha;
}
