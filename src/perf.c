#include "perf.h"

#include "debug.h"
#include "fen.h"
#include "generate.h"
#include "state.h"
#include "structs.h"

#include <stdio.h>
#include <stdlib.h>

uint64_t perft(uint32_t depth, uint32_t side) {
   if (!depth) { return 1; }

   uint64_t nmoves = 0;
   move_array_t moves = generate(side);
   for (uint32_t i = 0; i != moves.count; ++i) {
      advance(moves.data[i]);
      if (!in_check(side))
         nmoves += perft(depth - 1, side ^ 0x8);
      retract(moves.data[i]);
   }
   free(moves.data);

   return nmoves;
}

uint64_t perft_capture(uint32_t depth, uint32_t side) {
   if (!depth) { return 1; }

   uint64_t nmoves = 0;
   move_array_t moves = generate_captures(side);
   for (uint32_t i = 0; i != moves.count; ++i) {
      advance(moves.data[i]);
      if (!in_check(side))
         nmoves += perft_capture(depth - 1, side ^ 0x8);
      retract(moves.data[i]);
   }
   free(moves.data);

   return nmoves;
}

void trace(uint32_t side) {
   ttentry_t entry = {0};
   for (uint32_t t = 0; t != BASKETS; ++t) {
      uint32_t index = (hash_state & HASHMASK) ^ t;
      if (ttable[index]._.hash == hash_state >> HASHBITS
            && ttable[index]._.flags == FEXACT) {
         entry = ttable[index];
         break;
      }
   }

   move_t next = entry._.move;
   if (next.bits && is_legal(next, side)) {
      advance(next);
      if (!in_check(side)) {
         info_transposition_table_entry(entry, '\n');
         if (step > 3 && htable[step & 0x7] == htable[(step & 0x7) ^ 0x4])
            printf("  # (%c) infinite repetition!\n", fen_side[!side]);
         else
            trace(side ^ 0x8);
      } else {
         printf("  # (%c) lost!\n", fen_side[side >> 3]);
      }
      retract(next);
   }
}
