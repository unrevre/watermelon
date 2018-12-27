#include "generate.h"

#include "inlines.h"
#include "masks.h"
#include "state.h"

#include <stdlib.h>

move_array_t generate(uint32_t side) {
   move_array_t moves = {(move_t*)malloc(111 * sizeof(move_t)), 0};

   uint32_t s = side >> 3;

   __uint128_t C3U128 = 0x3;

   __uint128_t jset = GAME.pieces[side];
   while (jset) {
      uint32_t index = bsf_branchless(jset);

      __uint128_t rset, rlow, rcbn;
      __uint128_t fset, flow, fcbn;
      __uint128_t moveset;

      __uint128_t outer = (~GAME.empty ^ PMASK[index]) | OMASK[index];

      rlow = outer & LMASK[index];
      rcbn = C3U128 << bsr_branchless(rlow);
      rset = outer ^ (outer - rcbn);

      fset = outer & FMASK[index];
      flow = fset & LMASK[index];
      fcbn = C3U128 << bsr_branchless(flow);
      fset = fset ^ (fset - fcbn);

      moveset = (rset & RMASK[index]) | (fset & FMASK[index]);
      moveset &= ~GAME.occupancy[s];

      add_piecewise(moveset, index, &moves);
      jset = jset & (jset - 1);
   }

   __uint128_t mset = GAME.pieces[side + 1];
   while (mset) {
      uint32_t index = bsf_branchless(mset);

      __uint128_t moveset;
      moveset = (PMASK[index] << 0x11 & GAME.empty << 0x8 & FMASKN8)
         | (PMASK[index] >> 0x11 & GAME.empty >> 0x8 & FMASKN0)
         | (PMASK[index] << 0x13 & GAME.empty << 0xa & FMASKN0)
         | (PMASK[index] >> 0x13 & GAME.empty >> 0xa & FMASKN8)
         | (PMASK[index] << 0x07 & GAME.empty << 0x8 & FMASKN78)
         | (PMASK[index] >> 0x07 & GAME.empty >> 0x8 & FMASKN01)
         | (PMASK[index] << 0x0b & GAME.empty << 0xa & FMASKN01)
         | (PMASK[index] >> 0x0b & GAME.empty >> 0xa & FMASKN78);
      moveset &= BMASK & ~GAME.occupancy[s];

      add_piecewise(moveset, index, &moves);
      mset = mset & (mset - 1);
   }

   __uint128_t pset = GAME.pieces[side + 2];
   while (pset) {
      uint32_t index = bsf_branchless(pset);

      __uint128_t rset, rlow, rcbn;
      __uint128_t fset, flow, fcbn;
      __uint128_t moveset;

      __uint128_t occupancy = (~GAME.empty ^ PMASK[index]);
      __uint128_t outer = occupancy | OMASK[index];

      rlow = outer & LMASK[index];
      rcbn = C3U128 << bsr_branchless(rlow);
      rset = outer ^ (outer - rcbn);

      fset = outer & FMASK[index];
      flow = fset & LMASK[index];
      fcbn = C3U128 << bsr_branchless(flow);
      fset = fset ^ (fset - fcbn);

      moveset = (rset & RMASK[index]) | (fset & FMASK[index]);
      moveset &= GAME.empty;

      rset = ~rset & occupancy;
      rlow = rset & LMASK[index];
      rcbn = C3U128 << bsr_branchless(rlow);
      rset &= ~(rset - rcbn);

      fset = (~fset & occupancy) & FMASK[index];
      flow = fset & LMASK[index];
      fcbn = C3U128 << bsr_branchless(flow);
      fset &= ~(fset - fcbn);

      moveset |= ((rset & RMASK[index]) | (fset & FMASK[index]))
         & GAME.occupancy[!s];

      add_piecewise(moveset, index, &moves);
      pset = pset & (pset - 1);
   }

   __uint128_t zset;
   zset = (GAME.pieces[side + 3] << 9) >> (18 * s);
   zset &= ZMASK[s] & ~GAME.occupancy[s];
   add_shiftwise(zset, 9 - 18 * s, &moves);

   zset = GAME.pieces[side + 3] << 1 & FMASKN0;
   zset &= ZMASK[s] & ~GAME.occupancy[s];
   add_shiftwise(zset, 1, &moves);

   zset = GAME.pieces[side + 3] >> 1 & FMASKN8;
   zset &= ZMASK[s] & ~GAME.occupancy[s];
   add_shiftwise(zset, -1, &moves);

   __uint128_t xset = GAME.pieces[side + 4];
   while (xset) {
      uint32_t index = bsf_branchless(xset);

      __uint128_t moveset;
      moveset = (PMASK[index] << 0x10 & GAME.empty << 0x8)
         | (PMASK[index] << 0x14 & GAME.empty << 0xa)
         | (PMASK[index] >> 0x10 & GAME.empty >> 0x8)
         | (PMASK[index] >> 0x14 & GAME.empty >> 0xa);
      moveset &= XMASK[s] & ~GAME.occupancy[s];

      add_piecewise(moveset, index, &moves);
      xset = xset & (xset - 1);
   }

   __uint128_t sset = GAME.pieces[side + 5];
   while (sset) {
      uint32_t index = bsf_branchless(sset);

      __uint128_t moveset;
      moveset = PMASK[index] << 0x8 | PMASK[index] << 0xa
         | PMASK[index] >> 0x8 | PMASK[index] >> 0xa;
      moveset &= SMASK[s] & ~GAME.occupancy[s];

      add_piecewise(moveset, index, &moves);
      sset = sset & (sset - 1);
   }

   {
      __uint128_t moveset;
      moveset = GAME.pieces[side + 6] << 9
         | GAME.pieces[side + 6] >> 9
         | GAME.pieces[side + 6] << 1
         | GAME.pieces[side + 6] >> 1;
      moveset &= JMASK[s] & ~GAME.occupancy[s];
      add_piecewise(moveset, bsf_branchless(GAME.pieces[side + 6]), &moves);
   }

   return moves;
}

void add_piecewise(__uint128_t set, uint32_t from, move_array_t* moves) {
   while (set) {
      uint8_t to = bsf_branchless(set);
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
      uint8_t to = bsf_branchless(set);
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
   uint32_t index = bsf_branchless(GAME.pieces[side + 6]);

   uint32_t os = side ^ 0x8;

   __uint128_t jset = GAME.pieces[os] | GAME.pieces[os + 6];
   __uint128_t jrset = jset & RMASK[index];
   __uint128_t jfset = jset & FMASK[index];

   __uint128_t jmask = ~(jset | PMASK[index] | GAME.empty);

   for (; jrset; jrset &= jrset - 1) {
      __uint128_t lsb = jrset & -jrset;
      __uint128_t range = ((lsb ^ (-lsb)) ^ UMASK[index]);
      range = range & jmask & RMASK[index];
      if (!range) { return 1; }
   }

   for (; jfset; jfset &= jfset - 1) {
      __uint128_t lsb = jfset & -jfset;
      __uint128_t range = ((lsb ^ (-lsb)) ^ UMASK[index]);
      range = range & jmask & FMASK[index];
      if (!range) { return 1; }
   }

   __uint128_t pset = GAME.pieces[os + 2];
   __uint128_t prset = pset & RMASK[index];
   __uint128_t pfset = pset & FMASK[index];

   __uint128_t xmask = PMASK[index] | GAME.empty;

   for (; prset; prset &= prset - 1) {
      __uint128_t lsb = prset & -prset;
      __uint128_t range = ((lsb ^ (-lsb)) ^ UMASK[index]);
      __uint128_t pxmask = ~(lsb | xmask);
      range = range & pxmask & RMASK[index];
      if (popcnt(range) == 1) { return 1; }
   }

   for (; pfset; pfset &= pfset - 1) {
      __uint128_t lsb = pfset & -pfset;
      __uint128_t range = ((lsb ^ (-lsb)) ^ UMASK[index]);
      __uint128_t pxmask = ~(lsb | xmask);
      range = range & pxmask & FMASK[index];
      if (popcnt(range) == 1) { return 1; }
   }

   __uint128_t mset = (PMASK[index] << 0x11 & GAME.empty << 0x9)
      | (PMASK[index] >> 0x11 & GAME.empty >> 0x9)
      | (PMASK[index] << 0x13 & GAME.empty << 0x9)
      | (PMASK[index] >> 0x13 & GAME.empty >> 0x9)
      | (PMASK[index] << 0x07 & GAME.empty >> 0x1)
      | (PMASK[index] >> 0x07 & GAME.empty << 0x1)
      | (PMASK[index] << 0x0b & GAME.empty << 0x1)
      | (PMASK[index] >> 0x0b & GAME.empty >> 0x1);
   mset = mset & GAME.pieces[os + 1];
   if (mset) { return 1; }

   __uint128_t zset = (PMASK[index] << 9) >> (18 * side >> 3)
      | PMASK[index] << 1 | PMASK[index] >> 1;
   zset = zset & GAME.pieces[os + 3];
   if (zset) { return 1; }

   return 0;
}

void move(move_t move) {
   uint32_t side = move.internal.pfrom & 0x8;
   GAME.pieces[move.internal.pfrom] ^=
      PMASK[move.internal.from] | PMASK[move.internal.to];
   GAME.pieces[move.internal.pto] ^= PMASK[move.internal.to];
   GAME.pieces[7] = 0x0;

   GAME.occupancy[side >> 3] ^=
      PMASK[move.internal.from] | PMASK[move.internal.to];
   GAME.occupancy[!side] ^=
      GAME.occupancy[0] & GAME.occupancy[1];
   GAME.empty = ~(GAME.occupancy[0] | GAME.occupancy[1]);

   board[move.internal.from] = 7;
   board[move.internal.to] = move.internal.pfrom;
}

void retract(move_t move) {
   uint32_t side = move.internal.pfrom & 0x8;
   GAME.pieces[move.internal.pfrom] ^=
      PMASK[move.internal.from] | PMASK[move.internal.to];
   GAME.pieces[move.internal.pto] ^= PMASK[move.internal.to];
   GAME.pieces[7] = 0x0;

   GAME.occupancy[side >> 3] ^=
      PMASK[move.internal.from] | PMASK[move.internal.to];
   GAME.occupancy[!side] |= GAME.pieces[move.internal.pto];
   GAME.empty = ~(GAME.occupancy[0] | GAME.occupancy[1]);

   board[move.internal.from] = move.internal.pfrom;
   board[move.internal.to] = move.internal.pto;
}
