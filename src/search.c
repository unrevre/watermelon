#include "search.h"

#include "eval.h"
#include "generate.h"
#include "inlines.h"
#include "state.h"

#include <stdlib.h>

#ifdef DEBUG
uint32_t nodes;
uint32_t qnodes;

uint32_t tthits;
#endif

#ifdef TREE
#include "debug.h"

#include <stdio.h>
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
      printf("╻\n");
#endif
      int32_t score = negamax(d, 1, -2048, 2048, side);
#ifdef TREE
      printf("╹\n");
#endif

      if (int32t_abs(score) >= 2046 - d) { break; }
   }

   ttentry_t entry = {0};
   for (uint32_t t = 0; t != 4; ++t) {
      entry = TTABLE[(hash_state & 0xffffff) ^ t];
      if (entry._.hash == hash_state >> 24 && entry.bits) { break; }
   }

   return entry._.move;
}

int32_t negamax(uint32_t depth, uint32_t ply, int32_t alpha, int32_t beta,
                uint32_t side) {
#ifdef DEBUG
   ++nodes;
#endif

   alpha = max(alpha, -2049 + ply);
   beta = min(beta, 2048 - ply);

#ifdef TREE
   if (depth) { tree_node_entry(ply, alpha, beta, side); }
#endif

   int32_t alpha_parent = alpha;

   move_t move_hashed = {0};

   for (uint32_t t = 0; t < 4; ++t) {
      ttentry_t entry = TTABLE[(hash_state & 0xffffff) ^ t];
      if (entry._.hash == hash_state >> 24 && entry._.move.bits) {
         int32_t score = entry._.score;
         score = (score > 2049 - 32) ? score - ply :
            (score < -2049 + 32) ? score + ply : score;

         if (is_legal(entry._.move, side)) {
            move_hashed = entry._.move;

            if (entry._.depth >= depth) {
#ifdef DEBUG
               ++tthits;
#endif

               switch (entry._.flags) {
                  case 0x1:
                     return score;
                  case 0x2:
                     alpha = max(alpha, score);
                     break;
                  case 0x3:
                     beta = min(beta, score);
                     break;
               }

               if (alpha >= beta) { return score; }

               break;
            }
         }
      }
   }

   if (!depth) { return quiescence(ply, alpha, beta, side); }

   int32_t best = -2049 + ply;

   if (move_hashed.bits) {
      advance(move_hashed);

      int32_t score = -negamax(depth - 1, ply + 1, -beta, -alpha, side ^ 0x8);

      retract(move_hashed);

      best = max(best, score);
      alpha = max(alpha, score);

      if (alpha >= beta) {
         store_hash(depth, ply, alpha_parent, beta, score, move_hashed);
         return score;
      }
   }

   move_array_t moves = sort_moves(generate_pseudolegal(side));
   move_t move_store = (move_t){0};

   for (uint32_t i = 0; i != moves.quiet; ++i) {
      advance(moves.data[i]);

      int32_t score = -negamax(depth - 1, ply + 1, -beta, -alpha, side ^ 0x8);
#ifdef TREE
      tree_node_exit(ply, alpha, beta, score, side);
#endif

      retract(moves.data[i]);

      if (score > best) { move_store = moves.data[i]; }

      best = max(best, score);
      alpha = max(alpha, score);

      if (alpha >= beta) { break; }
   }

   move_t move_killer;
   for (uint32_t k = 0; k < 2; ++k) {
      move_killer = KTABLE[ply][k].move;
      if (move_killer.bits && is_legal(move_killer, side)) {
         advance(move_killer);
         int32_t score = -negamax(depth - 1, ply + 1, -beta, -alpha,
            side ^ 0x8);
         retract(move_killer);

         if (score > best) { move_store = move_killer; }

         best = max(best, score);
         alpha = max(alpha, score);

         if (alpha >= beta) {
            KTABLE[ply][k].count++;
            if (KTABLE[ply][1].count > KTABLE[ply][0].count) {
               KTABLE[ply][0] = KTABLE[ply][1];
               KTABLE[ply][1] = (killer_t){{0}, 0};
            }

            store_hash(depth, ply, alpha_parent, beta, best, move_store);
         }
      }
   }

   if (alpha < beta) {
      for (uint32_t i = moves.quiet; i != moves.count; ++i) {
         advance(moves.data[i]);

         int32_t score = -negamax(depth - 1, ply + 1, -beta, -alpha,
            side ^ 0x8);
#ifdef TREE
         tree_node_exit(ply, alpha, beta, score, side);
#endif

         retract(moves.data[i]);

         if (score > best) { move_store = moves.data[i]; }

         best = max(best, score);
         alpha = max(alpha, score);

         if (alpha >= beta) {
            if (!KTABLE[ply][0].move.bits) {
               KTABLE[ply][0].move = move_store;
            } else {
               if (!KTABLE[ply][1].count) {
                  KTABLE[ply][1].move = move_store;
                  KTABLE[ply][1].count++;
               } else {
                  KTABLE[ply][1].count--;
               }
            }

            break;
         }
      }
   }

   store_hash(depth, ply, alpha_parent, beta, best, move_store);

   free(moves.data);

   return best;
}

int32_t quiescence(uint32_t ply, int32_t alpha, int32_t beta, uint32_t side) {
#ifdef DEBUG
   ++qnodes;
#endif

   int32_t stand = eval(side);
#ifdef TREE
   tree_node_entry(ply, alpha, beta, side);
#endif
   if (stand >= beta) { return stand; }

   alpha = max(alpha, stand);

   move_array_t moves = sort_moves(generate_captures(side));
   for (uint32_t i = 0; i != moves.count; ++i) {
      advance(moves.data[i]);

      int32_t score = -quiescence(ply + 1, -beta, -alpha, side ^ 0x8);
#ifdef TREE
      tree_node_exit(ply, alpha, beta, score, side);
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
   uint32_t index = hash_state & 0xffffff;

   uint8_t flags = (int32t_abs(score) > 2049 - 32) ? 0x1 :
      (score <= alpha) ? 0x3 : (score >= beta) ? 0x2 : 0x1;

   score = (score > 2049 - 32) ? score + ply :
      (score < -2049 + 32) ? score - ply : score;

   uint32_t replace = 0x1000000;
   for (uint32_t t = 0; t != 4; ++t) {
      uint32_t entry = index ^ t;
      if (!TTABLE[entry].bits) {
         replace = entry;
         break;
      } else if (TTABLE[entry]._.hash == hash_state >> 24) {
         replace = entry;
         break;
      } else if (TTABLE[entry]._.age != age) {
         replace = entry;
      }
   }

   if (replace == 0x1000000) {
      uint32_t min_depth = 32;

      replace = index;
      for (uint32_t t = 0; t != 4; ++t) {
         uint32_t entry = index ^ t;
         if (TTABLE[entry]._.depth < min_depth) {
            min_depth = TTABLE[entry]._.depth;
            replace = entry;
         }
      }
   }

   TTABLE[replace] = (ttentry_t) {
      ._ = { hash_state >> 24, depth, flags, score, age, move_hashed }
   };
}
