#include "debug.h"

#include "eval.h"
#include "fen.h"
#include "magics.h"
#include "memory.h"
#include "position.h"
#include "state.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define size 40

void init_debug(debug_t* info) {
   info->buffer = calloc(PLYLIMIT * size + 1, sizeof(char));
   info->buffers = calloc(PLYLIMIT, sizeof(char*));

   for (int64_t i = 0; i < PLYLIMIT; ++i)
      info->buffers[i] = calloc(size + 1, sizeof(char));
}

void free_debug(debug_t* info) {
   for (int64_t i = 0; i < PLYLIMIT; ++i)
      free(info->buffers[i]);

   free(info->buffer);
   free(info->buffers);
}

/*!
 * impl_eval
 * @ internal implementation for 'info_eval'
 */

void impl_eval(char* buffer, int64_t side) {
   sprintf(buffer, "eval: [%c] %i", fen_char[side], eval(side));
}

char* info_eval(debug_t* info, int64_t side) {
   impl_eval(info->buffer, side);

   return info->buffer;
}

char* resp_eval(debug_t* info) {
   sprintf(info->buffer, "eval %i", eval(red));

   return info->buffer;
}

/*!
 * impl_fen
 * @ internal implementation for 'info_fen'
 */

void impl_fen(char* buffer) {
   memset(buffer, '0', 91);

   int64_t a = 81;
   int64_t f = 0;
   for (int64_t i = 0; i < 10; ++i) {
      int64_t g = f;
      for (int64_t j = 0; j < 9; ++a, ++j) {
         if (f == g) { ++f; }
         if (board[a] == empty) {
            buffer[g]++;
         } else {
            if (buffer[g] == '0') { f = g; }
            buffer[f++] = fen_char[board[a]];
            g = f;
         }
      }

      buffer[f++] = '/';
      a = a - 18;
   }

   buffer[--f] = ' ';
   buffer[++f] = fen_side[state.side];
   buffer[++f] = '\0';
}

char* info_fen(debug_t* info) {
   impl_fen(info->buffer);

   return info->buffer;
}

/*!
 * impl_game_state
 * @ internal implementation for 'info_game_state'
 */

void impl_game_state(char* buffer) {
   int64_t a = 81;
   int64_t g = 0;
   for (int64_t i = 0; i < 10; ++i) {
      char filler = (i == 4 || i == 5) ? '-' : ' ';
      for (int64_t j = 0; j < 9; ++a, ++j) {
         buffer[g++] = filler;
         buffer[g++] = (board[a] == empty) ? filler : fen_char[board[a]];
      }

      buffer[g++] = filler;
      buffer[g++] = '\n';
      a = a - 18;
   }

   buffer[g++] = '\0';
}

char* info_game_state(debug_t* info) {
   impl_game_state(info->buffer);

   return info->buffer;
}

/*!
 * impl_move
 * @ internal implementation for 'info_move'
 */

void impl_move(char* buffer, move_t move) {
   sprintf(buffer, "%c: %2i - %2i [%c]",
      fen_char[move._.pfrom], move._.from, move._.to, fen_char[move._.pto]);
}

char* info_move(debug_t* info, move_t move) {
   impl_move(info->buffer, move);

   return info->buffer;
}

char* resp_move(debug_t* info, move_t move) {
   sprintf(info->buffer, "move %i %i", move._.from, move._.to);

   return info->buffer;
}

/*!
 * impl_transposition_table_entry
 * @ internal implementation for 'info_transposition_table_entry'
 */

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

         **(buffer + 1) = '\0';
         impl_transposition_table_entry(*buffer, entry);
         if (is_repetition()) {
            strcat(*buffer, " %\n");
         } else {
            strcat(*buffer, "  \n");
            trace_principal_variation(++buffer);
         }

         retract(next);
      } else if (state.ply) {
         --buffer;
         (*buffer)[size - 2] = '#';
      }
   }
}

char* info_principal_variation(debug_t* info) {
   info->buffers[0][0] = '\0';
   trace_principal_variation(info->buffers);

   info->buffer[0] = '\0';
   for (int64_t i = 0; i < PLYLIMIT && info->buffers[i][0]; ++i)
      strcat(info->buffer, info->buffers[i]);

   return info->buffer;
}

#ifdef DEBUG
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdarg.h>

uint64_t nodes;
uint64_t qnodes;

uint64_t tthits;

void debug_variable_reset(int64_t count, ...) {
   va_list args;
   va_start(args, count);
   for (int64_t i = 0; i < count; ++i)
      *(va_arg(args, uint64_t*)) = 0;
   va_end(args);
}

void debug_variable_increment(int64_t count, ...) {
   va_list args;
   va_start(args, count);
   for (int64_t i = 0; i < count; ++i)
      ++(*(va_arg(args, uint64_t*)));
   va_end(args);
}

void debug_variable_headers(int64_t count, ...) {
   va_list args;
   va_start(args, count);
   for (int64_t i = 0; i < count; ++i)
      printf("%16s | ", va_arg(args, const char*));
   printf("\n");
   va_end(args);
}

void debug_variable_values(int64_t count, ...) {
   va_list args;
   va_start(args, count);
   for (int64_t i = 0; i < count; ++i)
      printf("%16"PRIu64" | ", va_arg(args, uint64_t));
   printf("\n");
   va_end(args);
}

void debug_printf(char const* fmt, ...) {
   va_list args;
   va_start(args, fmt);
   vprintf(fmt, args);
   va_end(args);
}

#ifdef TREE
void tree_root_entry(void) {
   printf("╻\n");
}

void tree_root_exit(void) {
   printf("╹\n");
}

void tree_node_entry(int32_t alpha, int32_t beta) {
   for (int32_t t = 0; t < state.ply; ++t) { printf("│"); }
   char buffer[102]; impl_fen(buffer); printf("├┬╸%s\n", buffer);
   for (int32_t t = 0; t < state.ply + 1; ++t) { printf("│"); }
   printf("├╸      [%5i, %5i]\n", alpha, beta);
}

void tree_node_exit(int32_t alpha, int32_t beta, int32_t score) {
   for (int32_t t = 0; t < state.ply + 1; ++t) { printf("│"); }
   char buffer[102]; impl_fen(buffer); printf("├╸%s\n", buffer);
   for (int32_t t = 0; t < state.ply + 1; ++t) { printf("│"); }
   printf("└╸%5i [%5i, %5i]\n", -score, -beta, -alpha);
}
#endif /* TREE */
#endif /* DEBUG */
