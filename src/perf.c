#include "perf.h"

#include "generate.h"
#include "magics.h"
#include "state.h"

#include <stdlib.h>

int64_t perft(int32_t depth) {
   if (!depth) { return 1; }

   int64_t nmoves = 0;
   move_array_t moves = generate(state.side);
   for (int64_t i = 0; i != moves.count; ++i) {
      advance(moves.data[i]);
      if (!in_check(state.side ^ pass))
         nmoves += perft(depth - 1);
      retract(moves.data[i]);
   }
   free(moves.data);

   return nmoves;
}

int64_t perft_capture(int32_t depth) {
   if (!depth) { return 1; }

   int64_t nmoves = 0;
   move_array_t moves = generate_captures(state.side);
   for (int64_t i = 0; i != moves.count; ++i) {
      advance(moves.data[i]);
      if (!in_check(state.side ^ pass))
         nmoves += perft_capture(depth - 1);
      retract(moves.data[i]);
   }
   free(moves.data);

   return nmoves;
}
