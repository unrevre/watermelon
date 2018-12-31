#include "search.h"

#include "eval.h"
#include "generate.h"
#include "inlines.h"
#include "state.h"

#include <stdlib.h>

#ifdef DEBUG
#include "fen.h"

#include <stdio.h>

uint32_t ply;

uint32_t nodes;
uint32_t qnodes;

uint32_t tthits;

static char cside[2] = {'r', 'b'};
#endif

move_t iter_dfs(uint32_t depth, uint32_t side) {
#ifdef DEBUG
   nodes = 0;
   qnodes = 0;

   tthits = 0;

   ply = 0;
#endif

   ++age;

   move_array_t moves = generate(side);
   move_t principal = {0};

   int32_t alpha = -2048; int32_t beta = 2048;
   for (uint32_t d = 1; d != depth; ++d) {
      int32_t high = -2048;

#ifdef DEBUG
      printf("╻\n");
#endif
      for (uint32_t i = 0; i != moves.count; ++i) {
         move(moves.data[i]);
#ifdef DEBUG
         ++ply;
         ++nodes;
#endif

         int32_t score = -negamax(d - 1, -beta, -alpha, side ^ 0x8);

#ifdef DEBUG
         for (uint32_t t = 0; t < ply; ++t) { printf("│"); }
         char* fen_str = info_fen();
         printf("├╸fen: %s\n", fen_str);
         free(fen_str);
         for (uint32_t t = 0; t < ply; ++t) { printf("│"); }
         printf("└╸(%c) [%i, %i] %i, %i\n", cside[side >> 3], alpha, beta,
            score, high);
#endif

         retract(moves.data[i]);
#ifdef DEBUG
         --ply;
#endif

         if (score > high) { principal = moves.data[i]; }
         high = max(high, score);
         alpha = max(alpha, score);

         if (alpha >= beta) { break; }
      }
#ifdef DEBUG
      printf("╹\n");
#endif

      if (high >= 2048) { break; }
   }

   free(moves.data);

   return principal;
}

int32_t negamax(uint32_t depth, int32_t alpha, int32_t beta, uint32_t side) {
#ifdef DEBUG
   if (depth) {
      for (uint32_t t = 0; t < ply - 1; ++t) { printf("│"); }
      char* fen_str = info_fen();
      printf("├┬╸fen: %s\n", fen_str);
      free(fen_str);
      for (uint32_t t = 0; t < ply; ++t) { printf("│"); }
      printf("├╸(%c) [%i, %i]\n", cside[side >> 3], alpha, beta);
   }
#endif

   move_t move_hash = {0};

   for (uint32_t t = 0; t < 4; ++t) {
      ttentry_t entry = TTABLE[(hash_state & 0xffffff) ^ t];
      if (entry.internal.hash == hash_state >> 24 &&
            entry.internal.depth >= depth && entry.internal.move.bits) {
#ifdef DEBUG
         ++tthits;
#endif
         switch (entry.internal.flags) {
            case 0x1:
               return entry.internal.score;
            case 0x2:
               alpha = max(alpha, entry.internal.score);
               break;
            case 0x3:
               beta = min(beta, entry.internal.score);
               break;
         }

         if (alpha >= beta) { return entry.internal.score; }

         move_hash = entry.internal.move;
         break;
      }
   }

   if (!depth) { return quiescence(alpha, beta, side); }

   int32_t high = -2048;

   if (move_hash.bits) {
      move(move_hash);

      int32_t score = -negamax(depth - 1, -beta, -alpha, side ^ 0x8);

      retract(move_hash);

      high = max(high, score);
      alpha = max(alpha, score);

      if (alpha >= beta) { store_hash(depth, alpha, beta, score, move_hash); }
   }

   uint32_t move_index = 0;
   move_array_t moves = generate(side);

   for (uint32_t i = 0; i != moves.count; ++i) {
      move(moves.data[i]);
#ifdef DEBUG
      ++ply;
      ++nodes;
#endif

      int32_t score = -negamax(depth - 1, -beta, -alpha, side ^ 0x8);
#ifdef DEBUG
      for (uint32_t t = 0; t < ply; ++t) { printf("│"); }
      char* fen_str = info_fen();
      printf("├╸fen: %s\n", fen_str);
      free(fen_str);
      for (uint32_t t = 0; t < ply; ++t) { printf("│"); }
      printf("└╸(%c) [%i, %i] %i, %i\n", cside[side >> 3], alpha, beta,
         score, high);
#endif

      retract(moves.data[i]);
#ifdef DEBUG
      --ply;
#endif

      if (score > high) { move_index = i; }

      high = max(high, score);
      alpha = max(alpha, score);

      if (alpha >= beta) { break; }
   }

   store_hash(depth, alpha, beta, high, moves.data[move_index]);

   free(moves.data);

   return high;
}

int32_t quiescence(int32_t alpha, int32_t beta, uint32_t side) {
   int32_t stand = eval(side);
#ifdef DEBUG
   for (uint32_t t = 0; t < ply - 1; ++t) { printf("│"); }
   char* fen_str = info_fen();
   printf("├┬╸[q] fen: %s\n", fen_str);
   free(fen_str);
   for (uint32_t t = 0; t < ply; ++t) { printf("│"); }
   printf("├╸[q] (%c) [%i, %i] %i\n", cside[side >> 3], alpha, beta, stand);
#endif
   if (stand >= beta) { return stand; }

   alpha = max(alpha, stand);

   move_array_t moves = generate_captures(side);
   for (uint32_t i = 0; i != moves.count; ++i) {
      move(moves.data[i]);
#ifdef DEBUG
      ++ply;
      ++qnodes;
#endif

      int32_t score = -quiescence(-beta, -alpha, side ^ 0x8);
#ifdef DEBUG
      for (uint32_t t = 0; t < ply; ++t) { printf("│"); }
      char* fen_str = info_fen();
      printf("├╸[q] fen: %s\n", fen_str);
      free(fen_str);
      for (uint32_t t = 0; t < ply; ++t) { printf("│"); }
      printf("└╸[q] (%c) [%i, %i] %i\n", cside[side >> 3], alpha, beta,
         score);
#endif

      retract(moves.data[i]);
#ifdef DEBUG
      --ply;
#endif

      alpha = max(alpha, score);
      if (alpha >= beta) { break; }
   }

   free(moves.data);

   return alpha;
}

void store_hash(uint32_t depth, int32_t alpha, int32_t beta, int32_t score,
                move_t move_hash) {
   uint32_t index = hash_state & 0xffffff;

   uint32_t replace = index;;
   for (uint32_t t = 0; t != 4; ++t) {
      uint32_t entry = index ^ t;
      if (!TTABLE[entry].bits) {
         replace = entry;
         break;
      } else if (TTABLE[entry].internal.hash == hash_state >> 24) {
         if (TTABLE[entry].internal.flags == 0x1
               && TTABLE[entry].internal.depth > depth)
            return;
         replace = entry;
         break;
      } else if (TTABLE[entry].internal.age != age) {
         replace = entry;
      }
   }

   uint8_t flags;
   if (score <= alpha) { flags = 0x3; }
   else if (score >= beta) { flags = 0x2; }
   else { flags = 0x1; }

   TTABLE[replace] = (ttentry_t) {
      .internal = { hash_state >> 24, depth, flags, score, age, move_hash }
   };
}
