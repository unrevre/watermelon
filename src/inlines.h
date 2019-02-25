#ifndef INLINES_H
#define INLINES_H

#include <stdint.h>

/*!
 * bsf_branchless
 * @ returns index of least significant bit of __uint128_t
 */

__inline__ uint64_t bsf_branchless(__uint128_t bits) {
   uint64_t clobber;

   uint64_t index;
   asm ("                  \n\
        movq   $64, %0     \n\
        bsfq   %3, %1      \n\
        cmovnz %1, %0      \n\
        addq   $64, %0     \n\
        bsfq   %2, %1      \n\
        cmovnz %1, %0      \n\
        "
        : "=&r" (index), "=&r" (clobber)
        : "r" (bits), "r" (bits >> 64)
        :
   );

   return index;
}

/*!
 * bsr_branchless
 * @ returns index of most significant bit of __uint128_t
 */

__inline__ uint64_t bsr_branchless(__uint128_t bits) {
   uint64_t clobber;

   uint64_t index;
   asm ("                  \n\
        movq   $128, %1    \n\
        bsrq   %2, %0      \n\
        cmovnz %0, %1      \n\
        xorq   $64, %1     \n\
        bsrq   %3, %0      \n\
        cmovz  %1, %0      \n\
        xorq   $64, %0     \n\
        "
        : "=&r" (index), "=&r" (clobber)
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

#endif /* INLINES_H */
