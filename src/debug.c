#include "debug.h"

#include "fen.h"
#include "memory.h"
#include "position.h"
#include "state.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define move_length 14
#define entry_length 28

static struct transient_t* state = 0;

void init_debug(struct debug_t* info) {
   memset(info, 0, sizeof(struct debug_t));

   for (int64_t i = 0; i != PLYLIMIT; ++i)
      info->buffers[i] = info->raw[i];

   state = &trunk;
}

void info_fen(char* buffer) {
   char* p = buffer;
   int64_t a = index_for(0, HEIGHT - 1);
   for (int64_t i = 0; i != RANKS; ++i) {
      char blanks = '0';
      for (int64_t j = 0; j != FILES; ++a, ++j) {
         if (state->board[a] == empty) {
            ++blanks;
         } else {
            *p++ = blanks;
            *p++ = fen_char[state->board[a]];
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

void info_game_state(char* buffer) {
   char* p = buffer;
   int64_t a = index_for(0, HEIGHT - 1);
   for (int64_t i = 0; i != RANKS; ++i) {
      char filler = (i == 4 || i == 5) ? '-' : ' ';
      for (int64_t j = 0; j != FILES; ++a, ++j) {
         *p++ = filler;
         *p++ = state->board[a] == empty
            ? filler : fen_char[state->board[a]];
      }

      *p++ = filler;
      *p++ = '\n';
      a = a - FILES - WIDTH;
   }

   *p = '\0';
}

void info_move(char* buffer, union move_t move) {
   sprintf(buffer, "move %2i %2i %c/%c",
           to_external(move._.from), to_external(move._.to),
           fen_char[move._.pfrom], fen_char[move._.pto]);
}

void info_transposition_table_entry(char* buffer, union ttentry_t entry) {
   info_move(buffer, entry._.move);
   sprintf(buffer + move_length, " %5i (%x)", entry._.score, entry._.flags);
}

void info_principal_variation(char** buffer, int64_t depth) {
   **buffer = '\0';
   union ttentry_t entry = entry_for_state(state);

   union move_t next = entry._.move;
   if (next.bits && is_valid(state, next)) {
      advance(next, state);
      if (!in_check(state, o(state->side))) {
         info_transposition_table_entry(*buffer, entry);
         (*buffer)[entry_length - 4] = ' ';
         (*buffer)[entry_length - 2] = '\n';

         if (entry._.depth < entry_for_state(state)._.depth) {
            (*buffer)[entry_length - 3] = '%';
            **++buffer = '\0';
         } else {
            (*buffer)[entry_length - 3] = ' ';
            info_principal_variation(++buffer, ++depth);
         }
      } else if (depth) {
         --buffer;
         (*buffer)[entry_length - 3] = '#';
      }
      retract(next, state);
   }
}

#ifdef INFO
uint64_t nodes;
uint64_t qnodes;
uint64_t tthits;

void debug_counter_reset(void) {
   nodes = qnodes = tthits = 0;
}
#endif /* INFO */

#ifdef TREE
#include <stdarg.h>

void tree_debug_state(struct transient_t* external) {
   state = external;
}

#define tree_node_depth(depth) do { \
   for (int32_t t = 0; t != depth; ++t) { printf("│"); } } while (0)

void tree_node_entry(int32_t alpha, int32_t beta) {
   tree_node_depth(state->ply + trunk.ply);
   char buffer[102]; info_fen(buffer); printf("├┬╸%s\n", buffer);
   tree_node_depth(state->ply + trunk.ply + 1);
   printf("├╸      [%5i, %5i]\n", alpha, beta);
}

void tree_node_exit(int32_t alpha, int32_t beta, int32_t score) {
   tree_node_depth(state->ply + trunk.ply + 1);
   char buffer[102]; info_fen(buffer); printf("├╸%s\n", buffer);
   tree_node_depth(state->ply + trunk.ply + 1);
   printf("└╸%5i [%5i, %5i]\n", -score, -beta, -alpha);
}

void tree_node_message(char const* fmt, ...) {
   tree_node_depth(state->ply + trunk.ply + 1); printf("├╸");
   va_list args; va_start(args, fmt); printf(fmt, args); va_end(args);
}
#endif /* TREE */
