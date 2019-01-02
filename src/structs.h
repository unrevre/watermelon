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
   } __attribute__((packed)) _;
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
 * ttentry_t
 * transposition table entry struct
 */

typedef union {
    uint64_t bits;
    struct {
        uint8_t hash  : 8;
        uint8_t depth : 6;
        uint8_t flags : 2;
        int16_t score : 14;
        uint8_t age   : 2;
        move_t move;
    } __attribute__((packed)) _;
} ttentry_t;

#endif /* STRUCTS_H */
