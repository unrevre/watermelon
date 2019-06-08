#ifndef STRUCTS_H
#define STRUCTS_H

#include "magics.h"

#include <setjmp.h>
#include <stdint.h>

/*!
 * move_t
 * @ move representation
 */

union move_t {
   uint32_t bits;
   struct {
      uint8_t from;
      uint8_t to;
      uint8_t pfrom;
      uint8_t pto;
   } __attribute__((packed)) _;
};

/*!
 * move_array_t
 * @ generated moves struct
 */

struct move_array_t {
   union move_t* data;
   int64_t count;
   int64_t quiet;
};

/*!
 * ttentry_t
 * @ transposition table entry struct
 */

union ttentry_t {
   uint64_t bits;
   struct {
      uint8_t hash  : 8;
      uint8_t flags : 2;
      uint8_t depth : 6;
      int16_t score : 14;
      uint8_t age   : 2;
      union move_t move;
   } __attribute__((packed)) _;
};

/*!
 * transient_t
 * @ transient variable struct
 */

struct transient_t {
   uint32_t hash;
   int32_t ply;
   int64_t side;
   __uint128_t pieces[PIECES];
   __uint128_t occupancy[2];
   uint32_t board[POINTS];
   uint32_t hashes[STEPLIMIT];
   jmp_buf env;
};

#endif /* STRUCTS_H */
