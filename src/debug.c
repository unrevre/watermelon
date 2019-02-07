#include "debug.h"

#include "fen.h"
#include "generate.h"
#include "inlines.h"
#include "magics.h"
#include "memory.h"
#include "state.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_debug(debug_t* info) {
   info->buffer = calloc(1281, sizeof(char));
   info->buffers = calloc(PLYLIMIT, sizeof(char*));

   for (uint32_t i = 0; i < PLYLIMIT; ++i)
      info->buffers[i] = calloc(41, sizeof(char));;
}

void free_debug(debug_t* info) {
   for (uint32_t i = 0; i < PLYLIMIT; ++i)
      free(info->buffers[i]);

   free(info->buffer);
   free(info->buffers);
}

void impl_fen(char* buffer) {
   uint32_t f = 0;
   for (uint32_t i = 0; i < 10; ++i) {
      uint32_t s = 0;
      for (uint32_t j = 0; j < 9; ++j) {
         if (board[(9 - i) * 9 + j] == empty) {
            ++s;
         } else {
            if (s) { buffer[f++] = '0' + s; s = 0; }
            buffer[f++] = fen_rep[board[(9 - i) * 9 + j]];
         }
      }

      if (s) { buffer[f++] = '0' + s; }
      buffer[f++] = '/';
   }

   buffer[--f] = ' ';
   buffer[++f] = fen_side[state.side];
   buffer[++f] = '\0';
}

char* info_fen(debug_t* info) {
   impl_fen(info->buffer);

   return info->buffer;
}

void impl_game_state(char* buffer) {
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
}

char* info_game_state(debug_t* info) {
   impl_game_state(info->buffer);

   return info->buffer;
}

void impl_move(char* buffer, move_t move) {
   sprintf(buffer, "%c: %2i - %2i [%c]",
      fen_rep[move._.pfrom], move._.from, move._.to, fen_rep[move._.pto]);
}

char* info_move(debug_t* info, move_t move) {
   impl_move(info->buffer, move);

   return info->buffer;
}

void impl_transposition_table_entry(char* buffer, ttentry_t entry) {
   impl_move(buffer, entry._.move);
   sprintf(buffer + strlen(buffer), "  %5i (%2u) [0x%x, 0x%x]",
      entry._.score, entry._.depth, entry._.flags, entry._.age);
}

char* info_transposition_table_entry(debug_t* info, ttentry_t entry) {
   impl_transposition_table_entry(info->buffer, entry);

   return info->buffer;
}

void trace_principal_variation(char** buffer) {
   ttentry_t entry = probe_hash_for_entry();

   move_t next = entry._.move;
   if (next.bits && is_valid(next, state.side)) {
      if (is_legal(next)) {
         advance(next);

         (*(buffer + 1))[0] = '\0';
         impl_transposition_table_entry(*buffer, entry);
         if (state.step > 4) {
            uint32_t curr = state.step & 0x7;
            uint32_t prev = (state.step - 1) & 0x7;
            if (htable[curr] == htable[curr ^ 0x4]
                  && htable[prev] == htable[prev ^ 0x4])
               strcat(*buffer, " %\n");
         } else {
            strcat(*buffer, "  \n");
            trace_principal_variation(++buffer);
         }
      } else {
         --buffer;
         (*buffer)[strlen(*buffer) - 2] = '#';
      }
      retract(next);
   }
}

char* info_principal_variation(debug_t* info) {
   info->buffers[0][0] = '\0';
   trace_principal_variation(info->buffers);

   info->buffer[0] = '\0';
   for (uint32_t i = 0; i < PLYLIMIT && info->buffers[i][0]; i++)
      strcat(info->buffer, info->buffers[i]);

   return info->buffer;
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
