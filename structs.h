#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdint.h>

/*!
 * state_t
 * game status
 * @ occupancy bitboards
 * @ piece bitboards
 */

typedef struct {
   __uint128_t occupancy[2];
   __uint128_t empty;

   __uint128_t pieces[15];
} state_t;

/*!
 * bsfq
 * - __uint64_t
 * = assembly instrinsic for bsfq
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
 * - __uint128_t
 * = returns index of least significant bit set
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
 * - __uint128_t
 * = returns index of least significant bit set
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
 * - __uint64_t
 * = assembly instrinsic for bsrq
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
 * - __uint128_t
 * = returns index of most significant bit set
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
 * - __uint128_t
 * = returns index of most significant bit set
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
 * - __uint128_t
 * @ implemented with intrinsics
 * + consider assembly
 */

__inline__ uint32_t popcnt(__uint128_t bits) {
   return __builtin_popcountll(bits >> 64) + __builtin_popcountll(bits);
}

/*!
 * move_t
 * move representation
 */

typedef union {
   uint32_t bits;
   struct {
      uint8_t from;
      uint8_t to;
      uint8_t pfrom;
      uint8_t pto;
   } __attribute__((packed)) internal;
} move_t;

/*!
 * move_array_t
 * generated moves struct
 */

typedef struct {
   move_t* data;
   uint32_t count;
} move_array_t;

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

/*!
 * abs
 * @ implementation for int32_t
 */

__inline__ int32_t int32t_abs(int32_t a) {
   int32_t s = a >> 31;
   return (a ^ s) - s;
}

#endif /* STRUCTS_H */
