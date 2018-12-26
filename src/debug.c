#include "debug.h"

#include <stdio.h>
#include <stdint.h>

void display(__uint128_t bits) {
   uint32_t b[90] = {0};

   __uint128_t bitmask = 0x1;
   for (uint32_t i = 0; i < 90; ++i) {
      if (bits & bitmask)
         b[i] = 1;
      bitmask = bitmask << 1;
   }

   for (uint32_t j = 10; j > 0; --j) {
      for (uint32_t k = 0; k < 9; ++k)
         printf("%i ", b[9 * (j - 1) + k]);
      printf("\n");
   }

   printf("\n");
}
