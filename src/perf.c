#include "perf.h"

#include "generate.h"
#include "magics.h"
#include "position.h"
#include "state.h"

#include <stdlib.h>

/*!
 * impl_perft
 * @ internal implementation of perft
 */

int64_t impl_perft(int32_t depth, int64_t side) {
   if (!depth) { return 1; }

   int64_t nmoves = 0;
   move_array_t moves = generate(side);
   for (int64_t i = 0; i != moves.count; ++i) {
      advance_board(moves.data[i]);
      if (!in_check(side))
         nmoves += impl_perft(depth - 1, side ^ pass);
      retract_board(moves.data[i]);
   }
   free(moves.data);

   return nmoves;
}

int64_t perft(int32_t depth) {
   return impl_perft(depth, trunk.side);
}

/*!
 * impl_perft_capture
 * @ internal implementation of perft_capture
 */

int64_t impl_perft_capture(int32_t depth, int64_t side) {
   if (!depth) { return 1; }

   int64_t nmoves = 0;
   move_array_t moves = generate_captures(side);
   for (int64_t i = 0; i != moves.count; ++i) {
      advance_board(moves.data[i]);
      if (!in_check(side))
         nmoves += impl_perft_capture(depth - 1, side ^ pass);
      retract_board(moves.data[i]);
   }
   free(moves.data);

   return nmoves;
}

int64_t perft_capture(int32_t depth) {
   return impl_perft_capture(depth, trunk.side);
}
