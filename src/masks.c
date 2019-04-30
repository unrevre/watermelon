#include "masks.h"

#include <stdint.h>

__uint128_t BMASK;

__uint128_t PMASK[BITS] __attribute__((aligned(64)));

__uint128_t UMASK[POINTS] __attribute__((aligned(64)));
__uint128_t LMASK[POINTS] __attribute__((aligned(64)));

__uint128_t RMASK[POINTS] __attribute__((aligned(64)));
__uint128_t FMASK[POINTS] __attribute__((aligned(64)));

__uint128_t OMASK[POINTS] __attribute__((aligned(64)));

__uint128_t JMASK[2];
__uint128_t SMASK[2];
__uint128_t XMASK[2];
__uint128_t ZMASK[2];

__uint128_t FMASKN0;
__uint128_t FMASKN8;
__uint128_t FMASKN01;
__uint128_t FMASKN78;

void init_masks(void) {
   BMASK = 0x3ffffff;
   BMASK = (BMASK << 64) + 0xffffffffffffffff;

   PMASK[0] = 0x1;
   for (uint32_t i = 1; i != POINTS; ++i)
      PMASK[i] = PMASK[i - 1] << 1;

   __uint128_t R0MASK = 0x1ff;
   __uint128_t F0MASK = 0x20100;
   F0MASK = (F0MASK << 64) + 0x8040201008040201;
   for (uint32_t i = 0; i != POINTS; ++i) {
      FMASK[i] = F0MASK << (i % 9);
      RMASK[i] = R0MASK << 9 * (i / 9);

      UMASK[i] = PMASK[i] ^ -PMASK[i];
      LMASK[i] = ~PMASK[i] & (PMASK[i] - 1);

      OMASK[i] = PMASK[i - (i % 9)] | PMASK[i - (i % 9) + 8]
         | PMASK[i % 9] | PMASK[81 + (i % 9)];
      OMASK[i] &= ~PMASK[i];
   }

   OMASK[0] |= 0x1;

   JMASK[0] = 0xe07038;
   JMASK[1] = 0x70381c;
   JMASK[1] = JMASK[1] << 64;

   SMASK[0] = 0xa02028;
   SMASK[1] = 0x501014;
   SMASK[1] = SMASK[1] << 64;

   XMASK[0] = 0x44004440044;
   XMASK[1] = 0x880088;
   XMASK[1] = (XMASK[1] << 64) + 0x8008800000000000;

   ZMASK[0] = 0x3ffffff;
   ZMASK[0] = (ZMASK[0] << 64) + 0xfffff55aa8000000;
   ZMASK[1] = 0x556abfffffffffff;

   FMASKN0 = ~FMASK[0];
   FMASKN8 = ~FMASK[8];
   FMASKN01 = ~(FMASK[0] | FMASK[1]);
   FMASKN78 = ~(FMASK[7] | FMASK[8]);
}
