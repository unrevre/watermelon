#include "search.h"

#include "bucket.h"
#include "core.h"
#include "debug.h"
#include "eval.h"
#include "generate.h"
#include "inlines.h"
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

struct killer_t ktable[PLYLIMIT] __attribute__((aligned(64)));

void iter_dfs(struct transient_t* state) {
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

int32_t negamax(int32_t depth, struct transient_t* state, int32_t alpha,
                int32_t beta, uint32_t principal) {
   debug_counter_increment(nodes);

   if (!search.status) { longjmp(state->env, TIMEOUT); }

   alpha = alpha < INFMINUS + state->ply ? INFMINUS + state->ply : alpha;
   beta = beta > INFPLUS - state->ply ? INFPLUS - state->ply : beta;
   if (alpha >= beta) { return alpha; }

   if (is_repetition(state)) { return DRAW; }

   int32_t alpha_parent = alpha;
   union move_t store = (union move_t){0};

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
      union move_t null = { ._ = { 0x7f, 0x7f, empty, empty } };

      advance(null, state);
      __builtin_prefetch(&ttable[state->hash & (HASHMASK ^ 0x3)], 1, 3);
      tree_node_entry(alpha, beta);
      int32_t score = -negamax(depth - 3, state, -beta, -beta + 1, 0);
      tree_node_exit(alpha, beta, score);
      retract(null, state);

      if (score >= beta) { return score; }
   }

   int32_t base = INFMINUS + state->ply;
   struct generator_t engine = { 0, 0, { 0, 0, 0, 0 }, store };

   int32_t best = base;
   union move_t move;

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

      struct killer_t* killers = &ktable[state->ply];
      switch (engine.state) {
         case 4:
            killers->count++;
            break;
         case 5:
            killers->count--;
            if (killers->count < 1) {
               killers->first = killers->second;
               killers->second = (union move_t){0};
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

   if (engine.state > 1) { release_slot(&bucket, engine.moves.index); }

   store_hash(state, depth, alpha_parent, beta, best, store);

   return best;
}

int32_t quiescence(struct transient_t* state, int32_t alpha, int32_t beta) {
   debug_counter_increment(qnodes);

   int32_t stand = eval(state, state->side);
   if (stand >= beta) { return stand; }
   alpha = stand > alpha ? stand : alpha;

   struct move_array_t moves = generate_captures(state);
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

   release_slot(&bucket, moves.index);

   return alpha;
}

union move_t next(struct generator_t* engine, struct transient_t* state) {
   switch (engine->state) {
      case 0:
         ++(engine->state);
         if (engine->move.bits)
            return engine->move;
      case 1:
         ++(engine->state);
         engine->moves = generate_pseudolegal(state);
         sort_moves(&(engine->moves));
      case 2:
         if (engine->index < engine->moves.quiet)
            return engine->moves.data[engine->index++];
      case 3:
         ++(engine->state);
         ++(engine->state);
         union move_t first = ktable[state->ply].first;
         if (first.bits && is_valid(state, first))
            return first;
      case 4:
         ++(engine->state);
         union move_t second = ktable[state->ply].second;
         if (second.bits && is_valid(state, second))
            return second;
      case 5:
         ++(engine->state);
      case 6:
         if (engine->index < engine->moves.count)
            return engine->moves.data[engine->index++];
      default:
         return (union move_t){0};
         break;
   }
}
