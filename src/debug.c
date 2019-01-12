#include "debug.h"

#include "fen.h"
#include "inlines.h"
#include "magics.h"
#include "state.h"

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

void debug_node_counts(uint32_t nodes, uint32_t qnodes, uint32_t tthits) {
   printf("nodes: %10u, qnodes: %10u, tthits: %10u\n", nodes, qnodes, tthits);
}

void tree_root_entry(void) {
   printf("╻\n");
}

void tree_root_exit(void) {
   printf("╹\n");
}

void tree_node_entry(uint32_t ply, int32_t alpha, int32_t beta, uint32_t side) {
   for (uint32_t t = 0; t < ply - 1; ++t) { printf("│"); }
   char* fen = info_fen(side); printf("├┬╸%s\n", fen);
   for (uint32_t t = 0; t < ply; ++t) { printf("│"); }
   printf("├╸      [%5i, %5i]\n", alpha, beta);
   free(fen);
}

void tree_node_exit(uint32_t ply, int32_t alpha, int32_t beta, int32_t score,
                    uint32_t side) {
   for (uint32_t t = 0; t < ply + 1; ++t) { printf("│"); }
   char* fen = info_fen(o(side)); printf("├╸%s\n", fen);
   for (uint32_t t = 0; t < ply + 1; ++t) { printf("│"); }
   printf("└╸%5i [%5i, %5i]\n", -score, -beta, -alpha);
   free(fen);
}
