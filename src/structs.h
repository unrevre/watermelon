#ifndef STRUCTS_H
#define STRUCTS_H

#include "magics.h"

#include <stdint.h>
#include <time.h>

/*!
 * state_t
 * @ game status
 */

typedef struct {
   __uint128_t pieces[PIECES];
   __uint128_t occupancy[2];
   uint32_t board[BITS];
} state_t;

/*!
 * move_t
 * @ move representation
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
 * @ generated moves struct
 */

typedef struct {
   move_t* data;
   int64_t count;
   int64_t quiet;
} move_array_t;

/*!
 * ttentry_t
 * @ transposition table entry struct
 */

typedef union {
   uint64_t bits;
   struct {
      uint8_t hash  : 8;
      uint8_t flags : 2;
      uint8_t depth : 6;
      int16_t score : 14;
      uint8_t age   : 2;
      move_t move;
   } __attribute__((packed)) _;
} ttentry_t;

/*!
 * killer_t
 * @ killer move struct
 */

typedef struct {
   move_t first;
   move_t second;
   int64_t count;
} killer_t;

/*!
 * transient_t
 * @ transient variable struct
 */

typedef struct {
   uint32_t hash;
   int32_t ply;
   int64_t side;
} transient_t;

/*!
 * generator_t
 * @ move ordering controller
 */

typedef struct {
   int64_t state;
   int64_t index;
   move_array_t moves;
   move_t move;
} generator_t;

/*!
 * debug_t
 * @ string buffers for informative/debugging purposes
 */

typedef struct {
   char* buffer;
   char** buffers;
} debug_t;

/*!
 * wmclock_t
 * @ struct for time management
 */

typedef struct {
   uint64_t status;
   double limit;
   time_t ref;
} wmclock_t;

/*!
 * search_t
 * @ search status information
 */

typedef struct {
   wmclock_t* clock;
   uint64_t nodes;
   uint64_t qnodes;
   uint64_t tthits;
} search_t;

#endif /* STRUCTS_H */
