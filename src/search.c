#include "search.h"

#include "eval.h"
#include "generate.h"
#include "inlines.h"

#ifdef DEBUG
#include "fen.h"

#include <stdio.h>

uint32_t nodes;
#endif

move_t iter_dfs(uint32_t depth, uint32_t side) {
#ifdef DEBUG
   nodes = 0;
#endif

   move_array_t moves = generate(side);
   move_t principal = {0};

   int32_t alpha = -4096; int32_t beta = 4096;
   for (uint32_t d = 1; d != depth; ++d) {
      int32_t high = -4096;
      for (uint32_t i = 0; i != moves.count; ++i) {
         move(moves.data[i]);
#ifdef DEBUG
         ++nodes;
#endif

         int32_t score = -negamax(d - 1, 1, -beta, -alpha, side ^ 0x8);

#ifdef DEBUG
         printf("fen: %s\n", info_fen());
         printf("  score: %i [%i, %i]\n", score, alpha, beta);
#endif

         retract(moves.data[i]);

         if (score > high) { principal = moves.data[i]; }
         high = max(high, score);
         alpha = max(alpha, score);

         if (alpha >= beta) { break; }
      }

      if (high >= 4096) { break; }
   }

   return principal;
}

int32_t negamax(uint32_t depth, uint32_t ply, int32_t alpha, int32_t beta,
                uint32_t side) {
   if (!depth) { return eval(side); }

   move_array_t moves = generate(side);

   int32_t high = -4096;
   for (uint32_t i = 0; i != moves.count; ++i) {
      move(moves.data[i]);
#ifdef DEBUG
      ++nodes;
#endif

      int32_t score = -negamax(depth - 1, ply + 1, -beta, -alpha, side ^ 0x8);

#ifdef DEBUG
      for (uint32_t t = 0; t < ply; ++t) { printf(" "); }
      printf("fen: %s\n", info_fen());
      for (uint32_t t = 0; t < ply; ++t) { printf(" "); }
      printf("  score: %i [%i, %i]\n", score, alpha, beta);
#endif

      retract(moves.data[i]);

      high = max(high, score);
      alpha = max(alpha, score);

      if (alpha >= beta) { break; }
   }

   return high;
}
