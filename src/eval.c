#include "eval.h"

#include "inlines.h"
#include "magics.h"

static int32_t weights[7] = {4096, 32, 16, 16, 4, 4, 4};

int32_t eval(struct transient_t* state, int32_t side) {
   int32_t popdiff = 0;
   for (int64_t i = 0; i != 7; ++i)
      popdiff += weights[i] * (popcnt(state->pieces[ps(red, i)])
         - popcnt(state->pieces[ps(black, i)]));

   return side ? -popdiff : popdiff;
}

int32_t gain(union move_t move) {
   return weights[p(move._.pto)];
}
