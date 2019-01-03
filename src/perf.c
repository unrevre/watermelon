#include "perf.h"

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

void trace(move_t move, uint32_t side) {
   move_t next = {0};
   int32_t score = 0;
   uint32_t flags = 0x0;

   advance(move);
   for (uint32_t t = 0; t != 4; ++t) {
      ttentry_t entry = TTABLE[(hash_state & 0xffffff) ^ t];
      if (entry._.hash == hash_state >> 24) {
         next = entry._.move;
         score = entry._.score;
         flags = entry._.flags;
         break;
      }
   }

   if (next.bits && is_legal(next, side ^ 0x8)) {
      printf("%2i: %2i - %2i, [%2i] %5i [0x%x]\n", next._.pfrom, next._.from,
         next._.to, next._.pto, score, flags);
      trace(next, side ^ 0x8);
   }

   retract(move);
   return;
}
