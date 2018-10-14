#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdint.h>

/*!
 * bitboard_t
 * board redisplayation
 * @ red pieces occupy bits 0 -- 8
 */

typedef union {
   __uint128_t bits;
   struct {
      uint64_t low  : 64;
      uint64_t high : 64;
   } __attribute__((packed)) internal;
} bitboard_t;

/*!
 * state_t
 * game status
 * @ occupancy bitboards
 * @ piece bitboards
 */

typedef struct {
   bitboard_t occupancy[2];
   bitboard_t empty;

   bitboard_t pieces[15];
} state_t;

/*!
 * bsf
 * - bitboard_t [__uint128_t]
 * = returns index of least significant bit set
 */

__inline__ uint32_t bsf(bitboard_t bitboard) {
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
        : "r" (bitboard.internal.low), "r" (bitboard.internal.high)
        :
   );

   return index;
}

/*!
 * bsr
 * - bitboard_t [__uint128_t]
 * = returns index of most significant bit set
 */

__inline__ uint32_t bsr(bitboard_t bitboard) {
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
        : "r" (bitboard.internal.low), "r" (bitboard.internal.high)
        :
   );

   return index;
}

/*!
 * popcnt
 * - bitboard_t [__uint128_t]
 * @ implemented with intrinsics
 * + consider assembly
 */

__inline__ uint32_t popcnt(bitboard_t bitfield) {
   return __builtin_popcountll(bitfield.internal.high)
      + __builtin_popcountll(bitfield.internal.low);
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