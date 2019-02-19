#include "eval.h"

#include "inlines.h"
#include "magics.h"
#include "state.h"

static int32_t weights[7] = {4096, 32, 16, 16, 4, 4, 4};

int32_t eval(int32_t side) {
   int32_t popdiff = 0;
   for (int64_t i = 0; i != 7; ++i)
      popdiff += weights[i] * (popcnt(game.pieces[ps(0x0, i)])
         - popcnt(game.pieces[po(0x0, i)]));

   return side ? -popdiff : popdiff;
}
