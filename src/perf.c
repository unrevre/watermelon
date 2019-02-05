#include "perf.h"

#include "generate.h"
#include "magics.h"
#include "state.h"

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
