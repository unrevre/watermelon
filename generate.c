#include "generate.h"

#include "masks.h"
#include "state.h"

#include <stdlib.h>

move_array_t generate(uint32_t side) {
   move_array_t moves = {(move_t*)malloc(111 * sizeof(move_t)), 0};

   uint32_t s = side >> 3;

   bitboard_t jset = GAME.pieces[side];
   while (jset.bits) {
      uint32_t index = bsf(jset);

      bitboard_t upper_f, lower_f, f_range;
      bitboard_t upper_r, lower_r, r_range;
      bitboard_t moveset;

      upper_r.bits = ~GAME.empty.bits & UMASK[index];
      lower_r.bits = ~GAME.empty.bits & LMASK[index];
      upper_f.bits = upper_r.bits & FMASK[index];
      lower_f.bits = lower_r.bits & FMASK[index];

      upper_r.bits &= -upper_r.bits;
      lower_r.bits = PMASK[0] << bsr(lower_r);
      r_range.bits = 2 * upper_r.bits - lower_r.bits;
      moveset.bits = r_range.bits & RMASK[index];

      upper_f.bits &= -upper_f.bits;
      lower_f.bits = PMASK[0] << bsr(lower_f);
      f_range.bits = 2 * upper_f.bits - lower_f.bits;
      moveset.bits |= f_range.bits & FMASK[index];

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

      bitboard_t upper_f_occ, lower_f_occ, lower_f_msb, upper_f_lsb;
      bitboard_t f_range, f_caps;
      bitboard_t upper_r_occ, lower_r_occ, lower_r_msb, upper_r_lsb;
      bitboard_t r_range, r_caps;

      bitboard_t moveset;

      upper_r_occ.bits = ~GAME.empty.bits & UMASK[index];
      lower_r_occ.bits = ~GAME.empty.bits & LMASK[index];
      upper_f_occ.bits = upper_r_occ.bits & FMASK[index];
      lower_f_occ.bits = lower_r_occ.bits & FMASK[index];

      upper_f_lsb.bits = upper_f_occ.bits & -upper_f_occ.bits;
      lower_f_msb.bits = PMASK[0] << bsr(lower_f_occ);
      f_range.bits = (upper_f_lsb.bits - lower_f_msb.bits) & GAME.empty.bits;

      upper_f_occ.bits ^= upper_f_lsb.bits;
      f_caps.bits = upper_f_occ.bits & -upper_f_occ.bits;
      lower_f_occ.bits ^= lower_f_msb.bits;
      f_caps.bits |= PMASK[0] << bsr(lower_f_occ) &
         ~(PMASK[index] - lower_f_msb.bits);
      f_range.bits |= f_caps.bits & GAME.occupancy[!side].bits;

      upper_r_lsb.bits = upper_r_occ.bits & -upper_r_occ.bits;
      lower_r_msb.bits = PMASK[0] << bsr(lower_r_occ);
      r_range.bits = (upper_r_lsb.bits - lower_r_msb.bits) & GAME.empty.bits;

      upper_r_occ.bits ^= upper_r_lsb.bits;
      r_caps.bits = upper_r_occ.bits & -upper_r_occ.bits;
      lower_r_occ.bits ^= lower_r_msb.bits;
      r_caps.bits |= PMASK[0] << bsr(lower_r_occ) &
         ~(PMASK[index] - lower_r_msb.bits);
      r_range.bits |= r_caps.bits & GAME.occupancy[!side].bits;

      moveset.bits = f_range.bits & FMASK[index];
      moveset.bits |= r_range.bits & RMASK[index];

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

   bitboard_t cross, jset, pset;
   cross.bits = FMASK[index] | RMASK[index];
   jset.bits = cross.bits
      & (GAME.pieces[side ^ 0x8].bits | GAME.pieces[(side ^ 0x8) + 6].bits);
   pset.bits = cross.bits & (GAME.pieces[(side ^ 0x8) + 2].bits);

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

   mset.bits &= GAME.pieces[(side ^ 0x8) + 1].bits;

   if (mset.bits)
      return 1;

   bitboard_t zset;
   zset.bits = (PMASK[index] << 9) >> (18 * side >> 3)
      | PMASK[index] << 1 | PMASK[index] >> 1;
   zset.bits &= GAME.pieces[(side ^ 0x8) + 3].bits;

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
