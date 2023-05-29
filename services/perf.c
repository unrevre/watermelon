#include "perf.h"

#include "bucket.h"
#include "generate.h"
#include "magics.h"
#include "position.h"
#include "state.h"
#include "structs.h"

int64_t perft(int32_t depth) {
   if (!depth) { return 1; }

   struct move_array_t moves = generate_pseudolegal(&trunk);
   trunk.side = o(trunk.side);

   int64_t nmoves = 0;
   for (int64_t i = 0; i != moves.count; ++i) {
      advance_board(moves.data[i], &trunk);
      if (!in_check(&trunk, o(trunk.side)))
         nmoves += perft(depth - 1);
      retract_board(moves.data[i], &trunk);
   }

   trunk.side = o(trunk.side);
   release_slot(&bucket, moves.index);

   return nmoves;
}

int64_t perft_capture(int32_t depth) {
   if (!depth) { return 1; }

   struct move_array_t moves = generate_captures(&trunk);
   trunk.side = o(trunk.side);

   int64_t nmoves = 0;
   for (int64_t i = 0; i != moves.count; ++i) {
      advance_board(moves.data[i], &trunk);
      if (!in_check(&trunk, o(trunk.side)))
         nmoves += perft_capture(depth - 1);
      retract_board(moves.data[i], &trunk);
   }

   trunk.side = o(trunk.side);
   release_slot(&bucket, moves.index);

   return nmoves;
}
