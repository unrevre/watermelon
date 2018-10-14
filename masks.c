#include "masks.h"

#include "structs.h"

#include <stdint.h>

__uint128_t NMASK;
__uint128_t BMASK;

__uint128_t PMASK[90] __attribute__((aligned(64)));

__uint128_t UMASK[90] __attribute__((aligned(64)));
__uint128_t LMASK[90] __attribute__((aligned(64)));

__uint128_t RMASK[90] __attribute__((aligned(64)));
__uint128_t FMASK[90] __attribute__((aligned(64)));

__uint128_t OMASK[90] __attribute__((aligned(64)));

__uint128_t JMASK[2];
__uint128_t SMASK[2];
__uint128_t XMASK[2];
__uint128_t ZMASK[2];

__uint128_t GMASK[2];

__uint128_t FMASKN0;
__uint128_t FMASKN8;
__uint128_t FMASKN01;
__uint128_t FMASKN78;

void init_masks(void) {
   bitboard_t bmask;
   bmask.internal.high = 0x3ffffff;
   bmask.internal.low  = 0xffffffffffffffff;
   BMASK = bmask.bits;

   PMASK[0] = 0x1;
   for (uint32_t i = 1; i < 90; ++i)
      PMASK[i] = PMASK[i - 1] << 1;

   bitboard_t R0MASK;
   R0MASK.bits = 0x1ff;

   bitboard_t F0MASK;
   F0MASK.internal.high = 0x20100;
   F0MASK.internal.low  = 0x8040201008040201;

   for (uint32_t i = 0; i < 90; ++i) {
      FMASK[i] = F0MASK.bits << (i % 9);
      RMASK[i] = R0MASK.bits << 9 * (i / 9);

      UMASK[i] = PMASK[i] ^ -PMASK[i];
      LMASK[i] = ~PMASK[i] & (PMASK[i] - 1);

      OMASK[i] = PMASK[i - (i % 9)] | PMASK[i - (i % 9) + 8]
         | PMASK[i % 9] | PMASK[81 + (i % 9)];
      OMASK[i] &= ~PMASK[i];
   }

   OMASK[0] |= 0x1;

   bitboard_t jmask[2];
   jmask[0].internal.low  = 0xe07038;
   jmask[1].internal.high = 0x70381c;
   JMASK[0] = jmask[0].bits;
   JMASK[1] = jmask[1].bits;

   bitboard_t smask[2];
   smask[0].internal.low  = 0xa02028;
   smask[1].internal.high = 0x501014;
   SMASK[0] = smask[0].bits;
   SMASK[1] = smask[1].bits;

   bitboard_t xmask[2];
   xmask[0].internal.low  = 0x44004440044;
   xmask[1].internal.high = 0x880088;
   xmask[1].internal.low  = 0x8008800000000000;
   XMASK[0] = xmask[0].bits;
   XMASK[1] = xmask[1].bits;

   bitboard_t zmask[2];
   zmask[0].internal.high = 0x3ffffff;
   zmask[0].internal.low  = 0xfffff55aa8000000;
   zmask[1].internal.low  = 0x556abfffffffffff;
   ZMASK[0] = zmask[0].bits;
   ZMASK[1] = zmask[1].bits;

   FMASKN0 = ~FMASK[0];
   FMASKN8 = ~FMASK[8];
   FMASKN01 = ~(FMASK[0] | FMASK[1]);
   FMASKN78 = ~(FMASK[7] | FMASK[8]);

   GMASK[0] = XMASK[0] | SMASK[0] | JMASK[0];
   GMASK[1] = XMASK[1] | SMASK[1] | JMASK[1];
}
