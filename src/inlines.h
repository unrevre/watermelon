#ifndef INLINES_H
#define INLINES_H

#include <stdint.h>

/*!
 * bsf
 * @ returns index of least significant bit of __uint128_t
 */

static __inline__ uint64_t bsf(__uint128_t bits) {
   uint64_t high = bits >> 64;
   uint64_t low = bits;

   uint64_t clobber;
   uint64_t index;
   __asm__ ("                  \n\
            tzcntq %3, %0      \n\
            addq   $64, %0     \n\
            tzcntq %2, %1      \n\
            cmovnc %1, %0      \n\
            "
            : "=&r" (index), "=&r" (clobber)
            : "r" (low), "r" (high)
            :
   );

   return index;
}

/*!
 * bsr
 * @ returns index of most significant bit of __uint128_t
 */

static __inline__ uint64_t bsr(__uint128_t bits) {
   uint64_t high = bits >> 64;
   uint64_t low = bits;

   uint64_t clobber;
   uint64_t index;
   __asm__ ("                  \n\
            movq   $191, %0    \n\
            lzcntq %2, %1      \n\
            cmovnc %1, %0      \n\
            addq   $64, %0     \n\
            lzcntq %3, %1      \n\
            cmovnc %1, %0      \n\
            xorq   $127, %0    \n\
            "
            : "=&r" (index), "=&r" (clobber)
            : "r" (low), "r" (high)
            :
   );

   return index;
}

/*!
 * popcnt
 * @ population count for __uint128_t
 * # implemented with compiler intrinsics
 */

static __inline__ int32_t popcnt(__uint128_t bits) {
   return __builtin_popcountll(bits >> 64) + __builtin_popcountll(bits);
}

/*!
 * atomaddl
 * @ atomic addition operation (long)
 */

static __inline__ void atomaddl(uint32_t* m, int32_t val) {
   __asm__ ("                  \n\
            lock; xaddl %0, %1 \n\
            "
            : "+r" (val), "+m" (*m)
            :
            : "memory"
   );
}

/*!
 * atomaddq
 * @ atomic addition operation (quadword)
 */

static __inline__ void atomaddq(uint64_t* m, int64_t val) {
   __asm__ ("                  \n\
            lock; xaddq %0, %1 \n\
            "
            : "+r" (val), "+m" (*m)
            :
            : "memory"
   );
}

#endif /* INLINES_H */
