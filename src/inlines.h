#ifndef INLINES_H
#define INLINES_H

#include <stdint.h>

#ifndef __clang__
/*!
 * bsfq
 * @ assembly instrinsic for bsfq
 */

__inline__ uint64_t bsfq(uint64_t bits) {
   uint64_t index;
   asm ("                  \n\
        bsfq   %1, %0      \n\
        "
        : "=rm" (index)
        : "rm" (bits)
        :
   );

   return index;
}

/*!
 * bsrq
 * @ assembly instrinsic for bsrq
 */

__inline__ uint64_t bsrq(uint64_t bits) {
   uint64_t index;
   asm ("                  \n\
        bsrq   %1, %0      \n\
        "
        : "=rm" (index)
        : "rm" (bits)
        :
   );

   return index;
}
#endif /* __clang__ */

/*!
 * bsf
 * @ returns index of least significant bit of __uint128_t
 */

__inline__ uint64_t bsf(__uint128_t bits) {
   uint64_t clobber;
   uint64_t index;
#ifndef __clang__
   uint64_t high = bits >> 64;
   uint64_t low = bits;

   clobber = bsfq(high);
   index = high ? clobber : 64;
   index = index + 64;
   clobber = bsfq(low);
   index = low ? clobber : index;
#else
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
#endif /* __clang__ */

   return index;
}

/*!
 * bsr
 * @ returns index of most significant bit of __uint128_t
 */

__inline__ uint64_t bsr(__uint128_t bits) {
   uint64_t clobber;
   uint64_t index;
#ifndef __clang__
   uint64_t high = bits >> 64;
   uint64_t low = bits;

   index = bsrq(low);
   clobber = low ? index : 128;
   clobber = clobber ^ 64;
   index = bsrq(high);
   index = !high ? clobber : index;
   index = index ^ 64;
#else
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
#endif /* __clang__ */

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
