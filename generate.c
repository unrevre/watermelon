#include "generate.h"

#include "masks.h"
#include "state.h"

#include <stdlib.h>

move_array_t generate(uint32_t side) {
   move_array_t moves = {(move_t*)malloc(111 * sizeof(move_t)), 0};

   uint32_t s = side >> 3;

   __uint128_t C3U128 = 0x3;

   bitboard_t jset = GAME.pieces[side];
   while (jset.bits) {
      uint32_t index = bsf(jset);

      bitboard_t rset, rloc, rcbn;
      bitboard_t fset, floc, fcbn;
      bitboard_t moveset;

      __uint128_t occupancy = (~GAME.empty.bits ^ PMASK[index]) | OMASK[index];

      rset.bits = occupancy;
      rloc.bits = rset.bits & LMASK[index];
      rcbn.bits = C3U128 << bsr(rloc);
      rset.bits = rset.bits ^ (rset.bits - rcbn.bits);

      fset.bits = occupancy & FMASK[index];
      floc.bits = fset.bits & LMASK[index];
      fcbn.bits = C3U128 << bsr(floc);
      fset.bits = fset.bits ^ (fset.bits - fcbn.bits);

      moveset.bits = (rset.bits & RMASK[index]) | (fset.bits & FMASK[index]);
      moveset.bits &= ~GAME.occupancy[s].bits;

      add_piecewise(moveset, index, &moves);
      jset.bits = jset.bits & (jset.bits - 1);
   }

   bitboard_t mset = GAME.pieces[side + 1];
   while (mset.bits) {
      uint32_t index = bsf(mset);

      bitboard_t moveset;
      moveset.bits = (PMASK[index] << 0x11 & GAME.empty.bits << 0x8 & FMASKN8)
         | (PMASK[index] >> 0x11 & GAME.empty.bits >> 0x8 & FMASKN0)
         | (PMASK[index] << 0x13 & GAME.empty.bits << 0xa & FMASKN0)
         | (PMASK[index] >> 0x13 & GAME.empty.bits >> 0xa & FMASKN8)
         | (PMASK[index] << 0x07 & GAME.empty.bits << 0x8 & FMASKN78)
         | (PMASK[index] >> 0x07 & GAME.empty.bits >> 0x8 & FMASKN01)
         | (PMASK[index] << 0x0b & GAME.empty.bits << 0xa & FMASKN01)
         | (PMASK[index] >> 0x0b & GAME.empty.bits >> 0xa & FMASKN78);
      moveset.bits &= BMASK & ~GAME.occupancy[s].bits;

      add_piecewise(moveset, index, &moves);
      mset.bits = mset.bits & (mset.bits - 1);
   }

   bitboard_t pset = GAME.pieces[side + 2];
   while (pset.bits) {
      uint32_t index = bsf(pset);

      bitboard_t rset, rloc, rcbn;
      bitboard_t fset, floc, fcbn;
      bitboard_t moveset;

      __uint128_t occupancy = (~GAME.empty.bits ^ PMASK[index]);
      __uint128_t oocc = occupancy | OMASK[index];

      rloc.bits = oocc & LMASK[index];
      rcbn.bits = C3U128 << bsr(rloc);
      rset.bits = oocc ^ (oocc - rcbn.bits);

      fset.bits = oocc & FMASK[index];
      floc.bits = fset.bits & LMASK[index];
      fcbn.bits = C3U128 << bsr(floc);
      fset.bits = fset.bits ^ (fset.bits - fcbn.bits);

      moveset.bits = (rset.bits & RMASK[index]) | (fset.bits & FMASK[index]);
      moveset.bits &= GAME.empty.bits;

      rset.bits = ~rset.bits & occupancy;
      rloc.bits = rset.bits & LMASK[index];
      rcbn.bits = C3U128 << bsr(rloc);
      rset.bits &= ~(rset.bits - rcbn.bits);

      fset.bits = (~fset.bits & occupancy) & FMASK[index];
      floc.bits = fset.bits & LMASK[index];
      fcbn.bits = C3U128 << bsr(floc);
      fset.bits &= ~(fset.bits - fcbn.bits);

      moveset.bits |= ((rset.bits & RMASK[index]) | (fset.bits & FMASK[index]))
         & GAME.occupancy[!s].bits;

      add_piecewise(moveset, index, &moves);
      pset.bits = pset.bits & (pset.bits - 1);
   }

   bitboard_t zset;
   zset.bits = (GAME.pieces[side + 3].bits << 9) >> (18 * s);
   zset.bits &= ZMASK[s] & ~GAME.occupancy[s].bits;
   add_shiftwise(zset, 9 - 18 * s, &moves);

   zset.bits = GAME.pieces[side + 3].bits << 1 & FMASKN0;
   zset.bits &= ZMASK[s] & ~GAME.occupancy[s].bits;
   add_shiftwise(zset, 1, &moves);

   zset.bits = GAME.pieces[side + 3].bits >> 1 & FMASKN8;
   zset.bits &= ZMASK[s] & ~GAME.occupancy[s].bits;
   add_shiftwise(zset, -1, &moves);

   bitboard_t xset = GAME.pieces[side + 4];
   while (xset.bits) {
      uint32_t index = bsf(xset);

      bitboard_t moveset;
      moveset.bits = (PMASK[index] << 0x10 & GAME.empty.bits << 0x8)
         | (PMASK[index] << 0x14 & GAME.empty.bits << 0xa)
         | (PMASK[index] >> 0x10 & GAME.empty.bits >> 0x8)
         | (PMASK[index] >> 0x14 & GAME.empty.bits >> 0xa);
      moveset.bits &= XMASK[s] & ~GAME.occupancy[s].bits;

      add_piecewise(moveset, index, &moves);
      xset.bits = xset.bits & (xset.bits - 1);
   }

   bitboard_t sset = GAME.pieces[side + 5];
   while (sset.bits) {
      uint32_t index = bsf(sset);

      bitboard_t moveset;
      moveset.bits = PMASK[index] << 0x8 | PMASK[index] << 0xa
         | PMASK[index] >> 0x8 | PMASK[index] >> 0xa;
      moveset.bits &= SMASK[s] & ~GAME.occupancy[s].bits;

      add_piecewise(moveset, index, &moves);
      sset.bits = sset.bits & (sset.bits - 1);
   }

   {
      bitboard_t moveset;
      moveset.bits = GAME.pieces[side + 6].bits << 9
         | GAME.pieces[side + 6].bits >> 9
         | GAME.pieces[side + 6].bits << 1
         | GAME.pieces[side + 6].bits >> 1;
      moveset.bits &= JMASK[s] & ~GAME.occupancy[s].bits;
      add_piecewise(moveset, bsf(GAME.pieces[side + 6]), &moves);
   }

   return moves;
}

void add_piecewise(bitboard_t set, uint32_t from, move_array_t* moves) {
   while (set.bits) {
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

      set.bits = set.bits & (set.bits - 1);
   }
}

void add_shiftwise(bitboard_t set, int32_t shift, move_array_t* moves) {
   while (set.bits) {
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

      set.bits = set.bits & (set.bits - 1);
   }
}

uint32_t in_check(uint32_t side) {
   uint32_t index = bsf(GAME.pieces[side + 6]);

   uint32_t os = side ^ 0x8;

   bitboard_t cross;
   cross.bits = FMASK[index] | RMASK[index];

   bitboard_t jset;
   jset.bits = (GAME.pieces[os].bits | GAME.pieces[os + 6].bits)
      & cross.bits;
   while (jset.bits) {
      bitboard_t hset, lset, btwn;
      uint32_t index_diff;

      uint32_t pt_index = bsf(jset);
      if (pt_index > index) {
         index_diff = pt_index - index;
         hset.bits = jset.bits & -jset.bits;
         lset.bits = GAME.pieces[side + 6].bits;
      } else {
         index_diff = index - pt_index;
         hset.bits = GAME.pieces[side + 6].bits;
         lset.bits = jset.bits & -jset.bits;
      }

      btwn.bits = hset.bits - lset.bits * 2;
      btwn.bits &= ~GAME.empty.bits;
      if (index_diff > 8)
         btwn.bits &= FMASK[index];

      if (!btwn.bits)
         return 1;

      jset.bits &= jset.bits - 1;
   }

   bitboard_t pset;
   pset.bits = GAME.pieces[os + 2].bits & cross.bits;
   while (pset.bits) {
      bitboard_t hset, lset, btwn;
      uint32_t index_diff;

      uint32_t pt_index = bsf(pset);
      if (pt_index > index) {
         index_diff = pt_index - index;
         hset.bits = pset.bits & -pset.bits;
         lset.bits = GAME.pieces[side + 6].bits;
      } else {
         index_diff = index - pt_index;
         hset.bits = GAME.pieces[side + 6].bits;
         lset.bits = pset.bits & -pset.bits;
      }

      btwn.bits = hset.bits - lset.bits * 2;
      btwn.bits &= ~GAME.empty.bits;
      if (index_diff > 8)
         btwn.bits &= FMASK[index];

      if (popcnt(btwn) == 1)
         return 1;

      pset.bits &= pset.bits - 1;
   }

   bitboard_t mset;
   mset.bits = (PMASK[index] << 0x11 & GAME.empty.bits << 0x9)
      | (PMASK[index] >> 0x11 & GAME.empty.bits >> 0x9)
      | (PMASK[index] << 0x13 & GAME.empty.bits << 0x9)
      | (PMASK[index] >> 0x13 & GAME.empty.bits >> 0x9)
      | (PMASK[index] << 0x07 & GAME.empty.bits >> 0x1)
      | (PMASK[index] >> 0x07 & GAME.empty.bits << 0x1)
      | (PMASK[index] << 0x0b & GAME.empty.bits << 0x1)
      | (PMASK[index] >> 0x0b & GAME.empty.bits >> 0x1);

   mset.bits &= GAME.pieces[os + 1].bits;

   if (mset.bits)
      return 1;

   bitboard_t zset;
   zset.bits = (PMASK[index] << 9) >> (18 * side >> 3)
      | PMASK[index] << 1 | PMASK[index] >> 1;
   zset.bits &= GAME.pieces[os + 3].bits;

   if (zset.bits)
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
