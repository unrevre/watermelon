#include "generate.h"

#include "masks.h"
#include "state.h"

#include <stdlib.h>

move_array_t generate(uint32_t side) {
   move_array_t moves = {(move_t*)malloc(111 * sizeof(move_t)), 0};

   uint32_t s = side >> 3;

   __uint128_t C3U128 = 0x3;

   __uint128_t jset = GAME.pieces[side].bits;
   while (jset) {
      uint32_t index = bsf(jset);

      __uint128_t rset, rlow, rcbn;
      __uint128_t fset, flow, fcbn;
      __uint128_t moveset;

      __uint128_t outer = (~GAME.empty.bits ^ PMASK[index]) | OMASK[index];

      rlow = outer & LMASK[index];
      rcbn = C3U128 << bsr(rlow);
      rset = outer ^ (outer - rcbn);

      fset = outer & FMASK[index];
      flow = fset & LMASK[index];
      fcbn = C3U128 << bsr(flow);
      fset = fset ^ (fset - fcbn);

      moveset = (rset & RMASK[index]) | (fset & FMASK[index]);
      moveset &= ~GAME.occupancy[s].bits;

      add_piecewise(moveset, index, &moves);
      jset = jset & (jset - 1);
   }

   __uint128_t mset = GAME.pieces[side + 1].bits;
   while (mset) {
      uint32_t index = bsf(mset);

      __uint128_t moveset;
      moveset = (PMASK[index] << 0x11 & GAME.empty.bits << 0x8 & FMASKN8)
         | (PMASK[index] >> 0x11 & GAME.empty.bits >> 0x8 & FMASKN0)
         | (PMASK[index] << 0x13 & GAME.empty.bits << 0xa & FMASKN0)
         | (PMASK[index] >> 0x13 & GAME.empty.bits >> 0xa & FMASKN8)
         | (PMASK[index] << 0x07 & GAME.empty.bits << 0x8 & FMASKN78)
         | (PMASK[index] >> 0x07 & GAME.empty.bits >> 0x8 & FMASKN01)
         | (PMASK[index] << 0x0b & GAME.empty.bits << 0xa & FMASKN01)
         | (PMASK[index] >> 0x0b & GAME.empty.bits >> 0xa & FMASKN78);
      moveset &= BMASK & ~GAME.occupancy[s].bits;

      add_piecewise(moveset, index, &moves);
      mset = mset & (mset - 1);
   }

   __uint128_t pset = GAME.pieces[side + 2].bits;
   while (pset) {
      uint32_t index = bsf(pset);

      __uint128_t rset, rlow, rcbn;
      __uint128_t fset, flow, fcbn;
      __uint128_t moveset;

      __uint128_t occupancy = (~GAME.empty.bits ^ PMASK[index]);
      __uint128_t outer = occupancy | OMASK[index];

      rlow = outer & LMASK[index];
      rcbn = C3U128 << bsr(rlow);
      rset = outer ^ (outer - rcbn);

      fset = outer & FMASK[index];
      flow = fset & LMASK[index];
      fcbn = C3U128 << bsr(flow);
      fset = fset ^ (fset - fcbn);

      moveset = (rset & RMASK[index]) | (fset & FMASK[index]);
      moveset &= GAME.empty.bits;

      rset = ~rset & occupancy;
      rlow = rset & LMASK[index];
      rcbn = C3U128 << bsr(rlow);
      rset &= ~(rset - rcbn);

      fset = (~fset & occupancy) & FMASK[index];
      flow = fset & LMASK[index];
      fcbn = C3U128 << bsr(flow);
      fset &= ~(fset - fcbn);

      moveset |= ((rset & RMASK[index]) | (fset & FMASK[index]))
         & GAME.occupancy[!s].bits;

      add_piecewise(moveset, index, &moves);
      pset = pset & (pset - 1);
   }

   __uint128_t zset;
   zset = (GAME.pieces[side + 3].bits << 9) >> (18 * s);
   zset &= ZMASK[s] & ~GAME.occupancy[s].bits;
   add_shiftwise(zset, 9 - 18 * s, &moves);

   zset = GAME.pieces[side + 3].bits << 1 & FMASKN0;
   zset &= ZMASK[s] & ~GAME.occupancy[s].bits;
   add_shiftwise(zset, 1, &moves);

   zset = GAME.pieces[side + 3].bits >> 1 & FMASKN8;
   zset &= ZMASK[s] & ~GAME.occupancy[s].bits;
   add_shiftwise(zset, -1, &moves);

   __uint128_t xset = GAME.pieces[side + 4].bits;
   while (xset) {
      uint32_t index = bsf(xset);

      __uint128_t moveset;
      moveset = (PMASK[index] << 0x10 & GAME.empty.bits << 0x8)
         | (PMASK[index] << 0x14 & GAME.empty.bits << 0xa)
         | (PMASK[index] >> 0x10 & GAME.empty.bits >> 0x8)
         | (PMASK[index] >> 0x14 & GAME.empty.bits >> 0xa);
      moveset &= XMASK[s] & ~GAME.occupancy[s].bits;

      add_piecewise(moveset, index, &moves);
      xset = xset & (xset - 1);
   }

   __uint128_t sset = GAME.pieces[side + 5].bits;
   while (sset) {
      uint32_t index = bsf(sset);

      __uint128_t moveset;
      moveset = PMASK[index] << 0x8 | PMASK[index] << 0xa
         | PMASK[index] >> 0x8 | PMASK[index] >> 0xa;
      moveset &= SMASK[s] & ~GAME.occupancy[s].bits;

      add_piecewise(moveset, index, &moves);
      sset = sset & (sset - 1);
   }

   {
      __uint128_t moveset;
      moveset = GAME.pieces[side + 6].bits << 9
         | GAME.pieces[side + 6].bits >> 9
         | GAME.pieces[side + 6].bits << 1
         | GAME.pieces[side + 6].bits >> 1;
      moveset &= JMASK[s] & ~GAME.occupancy[s].bits;
      add_piecewise(moveset, bsf(GAME.pieces[side + 6].bits), &moves);
   }

   return moves;
}

void add_piecewise(__uint128_t set, uint32_t from, move_array_t* moves) {
   while (set) {
      uint8_t to = bsf(set);
      move_t move = {
         .internal = {
            from,
            to,
            board[from],
            board[to],
         }
      };
      moves->data[moves->count++] = move;

      set = set & (set - 1);
   }
}

void add_shiftwise(__uint128_t set, int32_t shift, move_array_t* moves) {
   while (set) {
      uint8_t to = bsf(set);
      uint8_t from = to - shift;
      move_t move = {
         .internal = {
            from,
            to,
            board[from],
            board[to],
         }
      };
      moves->data[moves->count++] = move;

      set = set & (set - 1);
   }
}

uint32_t in_check(uint32_t side) {
   uint32_t index = bsf(GAME.pieces[side + 6].bits);

   uint32_t os = side ^ 0x8;

   __uint128_t cross;
   cross = FMASK[index] | RMASK[index];

   __uint128_t jset;
   jset = (GAME.pieces[os].bits | GAME.pieces[os + 6].bits) & cross;
   while (jset) {
      __uint128_t hset, lset, btwn;
      uint32_t index_diff;

      uint32_t pt_index = bsf(jset);
      if (pt_index > index) {
         index_diff = pt_index - index;
         hset = jset & -jset;
         lset = GAME.pieces[side + 6].bits;
      } else {
         index_diff = index - pt_index;
         hset = GAME.pieces[side + 6].bits;
         lset = jset & -jset;
      }

      btwn = hset - lset * 2;
      btwn &= ~GAME.empty.bits;
      if (index_diff > 8)
         btwn &= FMASK[index];

      if (!btwn)
         return 1;

      jset &= jset - 1;
   }

   __uint128_t pset;
   pset = GAME.pieces[os + 2].bits & cross;
   while (pset) {
      __uint128_t hset, lset, btwn;
      uint32_t index_diff;

      uint32_t pt_index = bsf(pset);
      if (pt_index > index) {
         index_diff = pt_index - index;
         hset = pset & -pset;
         lset = GAME.pieces[side + 6].bits;
      } else {
         index_diff = index - pt_index;
         hset = GAME.pieces[side + 6].bits;
         lset = pset & -pset;
      }

      btwn = hset - lset * 2;
      btwn &= ~GAME.empty.bits;
      if (index_diff > 8)
         btwn &= FMASK[index];

      if (popcnt(btwn) == 1)
         return 1;

      pset &= pset - 1;
   }

   __uint128_t mset;
   mset = (PMASK[index] << 0x11 & GAME.empty.bits << 0x9)
      | (PMASK[index] >> 0x11 & GAME.empty.bits >> 0x9)
      | (PMASK[index] << 0x13 & GAME.empty.bits << 0x9)
      | (PMASK[index] >> 0x13 & GAME.empty.bits >> 0x9)
      | (PMASK[index] << 0x07 & GAME.empty.bits >> 0x1)
      | (PMASK[index] >> 0x07 & GAME.empty.bits << 0x1)
      | (PMASK[index] << 0x0b & GAME.empty.bits << 0x1)
      | (PMASK[index] >> 0x0b & GAME.empty.bits >> 0x1);

   mset &= GAME.pieces[os + 1].bits;

   if (mset)
      return 1;

   __uint128_t zset;
   zset = (PMASK[index] << 9) >> (18 * side >> 3)
      | PMASK[index] << 1 | PMASK[index] >> 1;
   zset &= GAME.pieces[os + 3].bits;

   if (zset)
      return 1;

   return 0;
}

void move(move_t move) {
   uint32_t side = move.internal.pfrom & 0x8;
   GAME.pieces[move.internal.pfrom].bits ^=
      PMASK[move.internal.from] | PMASK[move.internal.to];
   GAME.pieces[move.internal.pto].bits ^= PMASK[move.internal.to];
   GAME.pieces[7].bits = 0x0;

   GAME.occupancy[side >> 3].bits ^=
      PMASK[move.internal.from] | PMASK[move.internal.to];
   GAME.occupancy[!side].bits ^=
      GAME.occupancy[0].bits & GAME.occupancy[1].bits;
   GAME.empty.bits = ~(GAME.occupancy[0].bits | GAME.occupancy[1].bits);

   board[move.internal.from] = 7;
   board[move.internal.to] = move.internal.pfrom;
}

void retract(move_t move) {
   uint32_t side = move.internal.pfrom & 0x8;
   GAME.pieces[move.internal.pfrom].bits ^=
      PMASK[move.internal.from] | PMASK[move.internal.to];
   GAME.pieces[move.internal.pto].bits ^= PMASK[move.internal.to];
   GAME.pieces[7].bits = 0x0;

   GAME.occupancy[side >> 3].bits ^=
      PMASK[move.internal.from] | PMASK[move.internal.to];
   GAME.occupancy[!side].bits |= GAME.pieces[move.internal.pto].bits;
   GAME.empty.bits = ~(GAME.occupancy[0].bits | GAME.occupancy[1].bits);

   board[move.internal.from] = move.internal.pfrom;
   board[move.internal.to] = move.internal.pto;
}
