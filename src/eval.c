#include "eval.h"

#include "inlines.h"
#include "state.h"

int32_t eval(uint32_t side) {
   int32_t popdiff = popcnt(GAME.occupancy[side >> 0x3])
      - popcnt(GAME.occupancy[!side]);

   return popdiff;
}
