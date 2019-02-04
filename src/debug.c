#include "debug.h"

#include "fen.h"
#include "inlines.h"
#include "magics.h"
#include "state.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

int32_t info_game_state(char* buffer) {
   char b[90] = {0};

   for (uint32_t i = 0x0; i != empty; ++i)
      for (__uint128_t bits = game.pieces[i]; bits; bits &= bits - 1)
         b[bsf_branchless(bits)] = fen_rep[i];

   uint32_t g = 0;
   for (uint32_t i = 10; i > 0; --i) {
      char filler = (i == 5 || i == 6) ? '-' : ' ';
      for (uint32_t j = 0; j < 9; ++j) {
         buffer[g++] = filler;
         buffer[g++] = (b[9 * (i - 1) + j]) ?
            b[9 * (i - 1) + j] : filler;
      }

      buffer[g++] = filler;
      buffer[g++] = '\n';
   }

   buffer[g++] = '\0';

   return g;
}

int32_t info_move(char* buffer, move_t move) {
   return sprintf(buffer, "%c: %2i - %2i [%c]",
      fen_rep[move._.pfrom], move._.from, move._.to, fen_rep[move._.pto]);
}

int32_t info_transposition_table_entry(char* buffer, ttentry_t entry) {
   int32_t offset = info_move(buffer, entry._.move);
   return offset + sprintf(buffer + offset, "  %5i (%2u) [0x%x, 0x%x]",
      entry._.score, entry._.depth, entry._.flags, entry._.age);
}

#ifdef DEBUG
uint32_t nodes;
uint32_t qnodes;

uint32_t tthits;

void debug_variable_reset(uint32_t count, ...) {
   va_list args;
   va_start(args, count);
   for (uint32_t i = 0; i < count; ++i)
      *(va_arg(args, uint32_t*)) = 0;
   va_end(args);
}

void debug_variable_increment(uint32_t count, ...) {
   va_list args;
   va_start(args, count);
   for (uint32_t i = 0; i < count; ++i)
      ++(*(va_arg(args, uint32_t*)));
   va_end(args);
}

void debug_variable_headers(uint32_t count, ...) {
   va_list args;
   va_start(args, count);
   for (uint32_t i = 0; i < count; ++i)
      printf("%16s | ", va_arg(args, const char*));
   printf("\n");
   va_end(args);
}

void debug_variable_values(uint32_t count, ...) {
   va_list args;
   va_start(args, count);
   for (uint32_t i = 0; i < count; ++i)
      printf("%16u | ", va_arg(args, uint32_t));
   printf("\n");
   va_end(args);
}
#endif /* DEBUG */

#ifdef TREE
void tree_root_entry(void) {
   printf("╻\n");
}

void tree_root_exit(void) {
   printf("╹\n");
}

void tree_node_entry(int32_t alpha, int32_t beta) {
   for (uint32_t t = 0; t < state.ply; ++t) { printf("│"); }
   char* fen = info_fen(); printf("├┬╸%s\n", fen);
   for (uint32_t t = 0; t < state.ply + 1; ++t) { printf("│"); }
   printf("├╸      [%5i, %5i]\n", alpha, beta);
   free(fen);
}

void tree_node_exit(int32_t alpha, int32_t beta, int32_t score) {
   for (uint32_t t = 0; t < state.ply + 1; ++t) { printf("│"); }
   char* fen = info_fen(); printf("├╸%s\n", fen);
   for (uint32_t t = 0; t < state.ply + 1; ++t) { printf("│"); }
   printf("└╸%5i [%5i, %5i]\n", -score, -beta, -alpha);
   free(fen);
}
#endif /* TREE */
