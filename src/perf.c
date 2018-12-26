#include "perf.h"

#include <stdlib.h>

uint64_t perft(uint32_t depth, uint32_t side) {
   if (!depth) { return 1; }

   uint64_t nmoves = 0;
   move_array_t moves = generate(side);
   for (uint32_t i = 0; i != moves.count; ++i) {
      move(moves.data[i]);
      if (!in_check(side))
         nmoves += perft(depth - 1, side ^ 0x8);
      retract(moves.data[i]);
   }
   free(moves.data);

   return nmoves;
}
