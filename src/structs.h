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

#endif /* STRUCTS_H */
