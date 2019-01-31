#include "debug.h"

#include "fen.h"
#include "inlines.h"
#include "magics.h"
#include "state.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void info_game_state(void) {
   char b[90] = {0};

   for (uint32_t i = 0; i < 90; ++i)
      b[i] = ' ';

   for (uint32_t i = 0x0; i < 0xf; ++i)
      for (__uint128_t bits = game.pieces[i]; bits; bits &= bits - 1)
         b[bsf_branchless(bits)] = fen_rep[i];

   printf("┎───────────────────┒\n");
   for (uint32_t i = 10; i > 0; --i) {
      switch (i) {
         case 5: case 6:
            printf("┠");
            for (uint32_t j = 0; j < 9; ++j) {
               if (b[9 * (i - 1) + j] == ' ')
                  printf("──");
               else
                  printf("─%c", b[9 * (i - 1) + j]);
            }
            printf("─┨\n");
            break;
         default:
            printf("┃");
            for (uint32_t j = 0; j < 9; ++j)
               printf(" %c", b[9 * (i - 1) + j]);
            printf(" ┃\n");
            break;
      }
   }
   printf("┖───────────────────┚\n");
   printf("\n");
}

void info_move(move_t move, char end) {
   printf("%c: %2i - %2i [%c]%c", fen_rep[move._.pfrom],
      move._.from, move._.to, fen_rep[move._.pto], end);
}

void info_transposition_table_entry(ttentry_t entry, char end) {
   info_move(entry._.move, ' ');
   printf(" %5i (%2u) [0x%x, 0x%x]%c", entry._.score, entry._.depth,
      entry._.flags, entry._.age, end);
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

void tree_node_entry(int32_t alpha, int32_t beta, int32_t offset) {
   for (uint32_t t = 0; t < state.ply + offset; ++t) { printf("│"); }
   char* fen = info_fen(); printf("├┬╸%s\n", fen);
   for (uint32_t t = 0; t < state.ply + offset + 1; ++t) { printf("│"); }
   printf("├╸      [%5i, %5i]\n", alpha, beta);
   free(fen);
}

void tree_node_exit(int32_t alpha, int32_t beta, int32_t score,
                    int32_t offset) {
   for (uint32_t t = 0; t < state.ply + offset + 1; ++t) { printf("│"); }
   char* fen = info_fen(); printf("├╸%s\n", fen);
   for (uint32_t t = 0; t < state.ply + offset + 1; ++t) { printf("│"); }
   printf("└╸%5i [%5i, %5i]\n", -score, -beta, -alpha);
   free(fen);
}
#endif /* TREE */
