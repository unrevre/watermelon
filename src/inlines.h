#ifndef INLINES_H
#define INLINES_H

#include <stdint.h>

/*!
 * bsfq
 * @ assembly instrinsic for bsfq
 */

__inline__ uint32_t bsfq(uint64_t bits) {
   uint64_t index;
   asm ("                  \n\
        bsfq   %1, %0      \n\
        "
        : "=r" (index)
        : "r" (bits)
        :
   );

   return index;
}

/*!
 * bsf_branchless
 * @ returns index of least significant bit set
 * # for __uint128_t (branchless version)
 */

__inline__ uint32_t bsf_branchless(__uint128_t bits) {
   uint64_t high = bits >> 64;
   uint64_t low = bits;
   uint32_t ret[3] = { bsfq(low), bsfq(high) + 64, 128 };
   uint32_t index = !low + ((!low) & (!high));
   return ret[index];
}

/*!
 * bsf
 * @ returns index of least significant bit set
 * # for __uint128_t
 */

__inline__ uint32_t bsf(__uint128_t bits) {
   uint64_t index;
   asm ("                  \n\
        bsfq   %1, %0      \n\
        jnz    1f          \n\
        bsfq   %2, %0      \n\
        jnz    2f          \n\
        movq   $64, %0     \n\
        2:                 \n\
        addq   $64, %0     \n\
        1:                 \n\
        "
        : "=r" (index)
        : "r" (bits), "r" (bits >> 64)
        :
   );

   return index;
}

/*!
 * bsrq
 * @ assembly instrinsic for bsrq
 */

__inline__ uint32_t bsrq(uint64_t bits) {
   uint64_t index;
   asm ("                  \n\
        bsrq   %1, %0      \n\
        "
        : "=r" (index)
        : "r" (bits)
        :
   );

   return index;
}

/*!
 * bsr_branchless
 * @ returns index of most significant bit set
 * # __uint128_t (branchless version)
 */

__inline__ uint32_t bsr_branchless(__uint128_t bits) {
   uint64_t high = bits >> 64;
   uint64_t low = bits;
   uint32_t ret[3] = { bsrq(high) + 64, bsrq(low), 128 };
   uint32_t index = !high + ((!low) & (!high));
   return ret[index];
}

/*!
 * bsr
 * @ returns index of most significant bit set
 * # __uint128_t
 */

__inline__ uint32_t bsr(__uint128_t bits) {
   uint64_t index;
   asm ("                  \n\
        bsrq   %2, %0      \n\
        jnz    2f          \n\
        bsrq   %1, %0      \n\
        jnz    1f          \n\
        movq   $64, %0     \n\
        2:                 \n\
        addq   $64, %0     \n\
        1:                 \n\
        "
        : "=r" (index)
        : "r" (bits), "r" (bits >> 64)
        :
   );

   return index;
}

/*!
 * popcnt
 * @ population count for __uint128_t
 * # implemented with compiler intrinsics
 */

__inline__ uint32_t popcnt(__uint128_t bits) {
   return __builtin_popcountll(bits >> 64) + __builtin_popcountll(bits);
}

/*!
 * max
 * @ implementation for int32_t
 */

__inline__ int32_t max(int32_t a, int32_t b) {
   int32_t diff = a - b;
   int32_t dsgn = diff >> 31;
   return a - (diff & dsgn);
}

/*!
 * min
 * @ implementation for int32_t
 */

__inline__ int32_t min(int32_t a, int32_t b) {
   int32_t diff = a - b;
   int32_t dsgn = diff >> 31;
   return b + (diff & dsgn);
}

#endif /* INLINES_H */
