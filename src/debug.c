#include "debug.h"

#include "eval.h"
#include "fen.h"
#include "magics.h"
#include "memory.h"
#include "position.h"
#include "state.h"

#include <stdio.h>
#include <stdlib.h>

#define move_length 14
#define entry_length 28

transient_t* state = 0;

void init_debug(debug_t* info) {
   info->buffer = calloc(256, sizeof(char));
   info->buffers = calloc(PLYLIMIT, sizeof(char*));

   for (int64_t i = 0; i < PLYLIMIT; ++i)
      info->buffers[i] = calloc(entry_length, sizeof(char));

   state = &trunk;
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

void impl_eval(char* buffer) {
   sprintf(buffer, "eval %i", eval(&trunk, red));
}

char* info_eval(debug_t* info) {
   impl_eval(info->buffer);

   return info->buffer;
}

/*!
 * impl_fen
 * @ internal implementation for 'info_fen'
 */

void impl_fen(char* buffer) {
   char* p = buffer;
   int64_t a = to_internal(0, HEIGHT - 1);
   for (int64_t i = 0; i != RANKS; ++i) {
      char blanks = '0';
      for (int64_t j = 0; j != FILES; ++a, ++j) {
         if (trunk.board[a] == empty) {
            ++blanks;
         } else {
            *p++ = blanks;
            *p++ = fen_char[trunk.board[a]];
            blanks = '0';
         }
      }

      *p++ = blanks;
      *p++ = '/';
      a = a - FILES - WIDTH;
   }

   --p;

   char* b = buffer;
   for (char* f = buffer; f != p; ++f, ++b) {
      *b = *f; if (*f == '0') { --b; } }

   *b++ = ' ';
   *b++ = fen_side[state->side];
   *b = '\0';
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
   char* p = buffer;
   int64_t a = to_internal(0, HEIGHT - 1);
   for (int64_t i = 0; i != RANKS; ++i) {
      char filler = (i == 4 || i == 5) ? '-' : ' ';
      for (int64_t j = 0; j != FILES; ++a, ++j) {
         *p++ = filler;
         *p++ = trunk.board[a] == empty
            ? filler : fen_char[trunk.board[a]];
      }

      *p++ = filler;
      *p++ = '\n';
      a = a - FILES - WIDTH;
   }

   *p = '\0';
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
   sprintf(buffer, "move %2i %2i %c/%c",
           to_external(move._.from), to_external(move._.to),
           fen_char[move._.pfrom], fen_char[move._.pto]);
}

char* info_move(debug_t* info, move_t move) {
   impl_move(info->buffer, move);

   return info->buffer;
}

/*!
 * impl_transposition_table_entry
 * @ internal implementation for 'info_transposition_table_entry'
 */

void impl_transposition_table_entry(char* buffer, ttentry_t entry) {
   impl_move(buffer, entry._.move);
   sprintf(buffer + move_length, " %5i (%x)", entry._.score, entry._.flags);
}

char* info_transposition_table_entry(debug_t* info, ttentry_t entry) {
   impl_transposition_table_entry(info->buffer, entry);

   return info->buffer;
}

/*!
 * trace_principal_variation
 * @ recursively trace principal variation line
 */

void trace_principal_variation(char** buffer) {
   **buffer = '\0';
   ttentry_t entry = probe_hash_for_entry(state);

   move_t next = entry._.move;
   if (next.bits && is_valid(state, next)) {
      advance(next, state);
      if (!in_check(state, o(state->side))) {
         impl_transposition_table_entry(*buffer, entry);
         (*buffer)[entry_length - 4] = ' ';
         (*buffer)[entry_length - 2] = '\n';
         if (is_repetition(state)) {
            (*buffer)[entry_length - 3] = '%';
            **++buffer = '\0';
         } else {
            (*buffer)[entry_length - 3] = ' ';
            trace_principal_variation(++buffer);
         }
      } else if (state->ply) {
         --buffer;
         (*buffer)[entry_length - 3] = '#';
      }
      retract(next, state);
   }
}

char** info_principal_variation(debug_t* info) {
   trace_principal_variation(info->buffers);

   return info->buffers;
}

#ifdef DEBUG
#define __STDC_FORMAT_MACROS
#include <stdarg.h>

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

void debug_printf(char const* fmt, ...) {
   va_list args;
   va_start(args, fmt);
   vprintf(fmt, args);
   va_end(args);
}

#ifdef TREE
void tree_debug_state(transient_t* external) {
   state = external;
}

void tree_root_entry(void) {
   printf("╻\n");
}

void tree_root_exit(void) {
   printf("╹\n");
}

void tree_node_entry(int32_t alpha, int32_t beta) {
   for (int32_t t = 0; t < state->ply; ++t) { printf("│"); }
   char buffer[102]; impl_fen(buffer); printf("├┬╸%s\n", buffer);
   for (int32_t t = 0; t < state->ply + 1; ++t) { printf("│"); }
   printf("├╸      [%5i, %5i]\n", alpha, beta);
}

void tree_node_message(char const* fmt, ...) {
   for (int32_t t = 0; t < state->ply + 1; ++t) { printf("│"); }
   printf("├╸"); va_list args; va_start(args, fmt); vprintf(fmt, args);
}

void tree_node_exit(int32_t alpha, int32_t beta, int32_t score) {
   for (int32_t t = 0; t < state->ply + 1; ++t) { printf("│"); }
   char buffer[102]; impl_fen(buffer); printf("├╸%s\n", buffer);
   for (int32_t t = 0; t < state->ply + 1; ++t) { printf("│"); }
   printf("└╸%5i [%5i, %5i]\n", -score, -beta, -alpha);
}
#endif /* TREE */
#endif /* DEBUG */
