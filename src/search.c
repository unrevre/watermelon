#include "search.h"

#include "eval.h"
#include "generate.h"
#include "inlines.h"

#include <stdlib.h>

#ifdef DEBUG
#include "fen.h"

#include <stdio.h>

uint32_t ply;

uint32_t nodes;
uint32_t qnodes;
#endif

move_t iter_dfs(uint32_t depth, uint32_t side) {
#ifdef DEBUG
   nodes = 0;
   qnodes = 0;
#endif

   move_array_t moves = generate(side);
   move_t principal = {0};

   int32_t alpha = -4096; int32_t beta = 4096;
   for (uint32_t d = 1; d != depth; ++d) {
      int32_t high = -4096;
      for (uint32_t i = 0; i != moves.count; ++i) {
         move(moves.data[i]);
#ifdef DEBUG
         ply = 0;
         ++nodes;
         char* fen_str = info_fen();
         printf("fen: %s\n", fen_str);
#endif

         int32_t score = -negamax(d - 1, -beta, -alpha, side ^ 0x8);

#ifdef DEBUG
         if (d > 1) { printf("fen: %s\n", fen_str); }
         free(fen_str);
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

   free(moves.data);

   return principal;
}

int32_t negamax(uint32_t depth, int32_t alpha, int32_t beta, uint32_t side) {
   if (!depth) { return quiescence(alpha, beta, side); }

   move_array_t moves = generate(side);

   int32_t high = -4096;
   for (uint32_t i = 0; i != moves.count; ++i) {
      move(moves.data[i]);
#ifdef DEBUG
      ++ply;
      ++nodes;
      for (uint32_t t = 0; t < ply; ++t) { printf(" "); }
      char* fen_str = info_fen();
      printf("fen: %s\n", fen_str);
#endif

      int32_t score = -negamax(depth - 1, -beta, -alpha, side ^ 0x8);

#ifdef DEBUG
      if (depth > 1) {
         for (uint32_t t = 0; t < ply; ++t) { printf(" "); }
         printf("fen: %s\n", fen_str);
      }
      free(fen_str);
      for (uint32_t t = 0; t < ply; ++t) { printf(" "); }
      printf("  score: %i [%i, %i]\n", score, alpha, beta);
      --ply;
#endif

      retract(moves.data[i]);

      high = max(high, score);
      alpha = max(alpha, score);

      if (alpha >= beta) { break; }
   }

   free(moves.data);

   return high;
}

int32_t quiescence(int32_t alpha, int32_t beta, uint32_t side) {
   int32_t stand = eval(side);
   if (stand >= beta) { return stand; }

   alpha = max(alpha, stand);

   move_array_t moves = generate_captures(side);
   for (uint32_t i = 0; i != moves.count; ++i) {
      move(moves.data[i]);
#ifdef DEBUG
      ++ply;
      ++qnodes;
      for (uint32_t t = 0; t < ply; ++t) { printf(" "); }
      char* fen_str = info_fen();
      printf("[q] fen: %s\n", fen_str);
#endif

      int32_t score = -quiescence(-beta, -alpha, side ^ 0x8);

#ifdef DEBUG
      for (uint32_t t = 0; t < ply; ++t) { printf(" "); }
      printf("[q] fen: %s\n", fen_str);
      free(fen_str);
      for (uint32_t t = 0; t < ply; ++t) { printf(" "); }
      printf("  [q] score: %i [%i, %i]\n", score, alpha, beta);
      --ply;
#endif

      retract(moves.data[i]);

      alpha = max(alpha, score);
      if (alpha >= beta) { break; }
   }

   free(moves.data);

   return alpha;
}
