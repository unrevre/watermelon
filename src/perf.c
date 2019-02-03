#include "perf.h"

#include "debug.h"
#include "fen.h"
#include "generate.h"
#include "magics.h"
#include "memory.h"
#include "state.h"
#include "structs.h"

#include <stdio.h>
#include <stdlib.h>

uint64_t perft(uint32_t depth) {
   if (!depth) { return 1; }

   uint64_t nmoves = 0;
   move_array_t moves = generate(state.side);
   for (uint32_t i = 0; i != moves.count; ++i) {
      advance(moves.data[i]);
      if (!in_check(o(state.side)))
         nmoves += perft(depth - 1);
      retract(moves.data[i]);
   }
   free(moves.data);

   return nmoves;
}

uint64_t perft_capture(uint32_t depth) {
   if (!depth) { return 1; }

   uint64_t nmoves = 0;
   move_array_t moves = generate_captures(state.side);
   for (uint32_t i = 0; i != moves.count; ++i) {
      advance(moves.data[i]);
      if (!in_check(o(state.side)))
         nmoves += perft_capture(depth - 1);
      retract(moves.data[i]);
   }
   free(moves.data);

   return nmoves;
}

void trace_principal_variation(void) {
   ttentry_t entry = {0};
   for (uint32_t t = 0; t != BASKETS; ++t) {
      uint32_t index = (state.hash & HASHMASK) ^ t;
      if (ttable[index]._.hash == state.hash >> HASHBITS
            && ttable[index]._.flags == FEXACT) {
         entry = ttable[index];
         break;
      }
   }

   move_t next = entry._.move;
   if (next.bits && is_legal(next, state.side)) {
      advance(next);
      if (!in_check(o(state.side))) {
         info_transposition_table_entry(entry, '\n');
         if (state.step > 4) {
            uint32_t curr = state.step & 0x7;
            uint32_t prev = (state.step - 1) & 0x7;
            if (htable[curr] == htable[curr ^ 0x4]
                  && htable[prev] == htable[prev ^ 0x4])
               printf("  # (%c) infinite repetition!\n",
                  fen_side[o(state.side)]);
         } else {
            trace_principal_variation();
         }
      } else {
         printf("  # (%c) lost!\n", fen_side[o(state.side)]);
      }
      retract(next);
   }
}
