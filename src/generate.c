#include "generate.h"

#include "inlines.h"
#include "magics.h"
#include "masks.h"
#include "memory.h"
#include "position.h"
#include "state.h"

#include <stdlib.h>

move_array_t generate(int64_t side) {
   move_array_t moves = {malloc(111 * sizeof(move_t)), 0, 0};

   __uint128_t C3U128 = 0x3;

   __uint128_t jset = game.pieces[ps(side, 0x1)];
   while (jset) {
      uint64_t index = bsf_branchless(jset);
      jset = jset ^ PMASK[index];

      __uint128_t rset, rlow, rcbn;
      __uint128_t fset, flow, fcbn;
      __uint128_t moveset;

      __uint128_t outer = (~game.pieces[empty] ^ PMASK[index]) | OMASK[index];

      rlow = outer & LMASK[index];
      rcbn = C3U128 << bsr_branchless(rlow);
      rset = outer ^ (outer - rcbn);

      flow = rlow & FMASK[index];
      fcbn = C3U128 << bsr_branchless(flow);
      fset = outer & FMASK[index];
      fset = fset ^ (fset - fcbn);

      moveset = (rset & RMASK[index]) | (fset & FMASK[index]);
      moveset &= ~game.occupancy[side];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t mset = game.pieces[ps(side, 0x2)];
   while (mset) {
      uint64_t index = bsf_branchless(mset);
      mset = mset ^ PMASK[index];

      __uint128_t moveset = 0;
      __uint128_t lset = PMASK[index] & FMASKN01 & (game.pieces[empty] << 1);
      moveset = moveset | lset << 7 | lset >> 11;
      __uint128_t rset = PMASK[index] & FMASKN78 & (game.pieces[empty] >> 1);
      moveset = moveset | rset << 11 | rset >> 7;
      __uint128_t flset = PMASK[index] & FMASKN0 & (game.pieces[empty] >> 9);
      moveset = moveset | flset << 17;
      __uint128_t frset = PMASK[index] & FMASKN8 & (game.pieces[empty] >> 9);
      moveset = moveset | frset << 19;
      __uint128_t blset = PMASK[index] & FMASKN0 & (game.pieces[empty] << 9);
      moveset = moveset | blset >> 19;
      __uint128_t brset = PMASK[index] & FMASKN8 & (game.pieces[empty] << 9);
      moveset = moveset | brset >> 17;
      moveset &= BMASK & ~game.occupancy[side];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t pset = game.pieces[ps(side, 0x3)];
   while (pset) {
      uint64_t index = bsf_branchless(pset);
      pset = pset ^ PMASK[index];

      __uint128_t rset, rlow, rcbn;
      __uint128_t fset, flow, fcbn;
      __uint128_t moveset;

      __uint128_t occupancy = (~game.pieces[empty] ^ PMASK[index]);
      __uint128_t outer = occupancy | OMASK[index];

      rlow = outer & LMASK[index];
      rcbn = C3U128 << bsr_branchless(rlow);
      rset = outer ^ (outer - rcbn);

      flow = rlow & FMASK[index];
      fcbn = C3U128 << bsr_branchless(flow);
      fset = outer & FMASK[index];
      fset = fset ^ (fset - fcbn);

      moveset = (rset & RMASK[index]) | (fset & FMASK[index]);
      moveset &= game.pieces[empty];

      rset = ~rset & occupancy;
      rlow = rset & LMASK[index];
      rcbn = C3U128 << bsr_branchless(rlow);
      rset &= ~(rset - rcbn);

      fset = (~fset & occupancy) & FMASK[index];
      flow = fset & LMASK[index];
      fcbn = C3U128 << bsr_branchless(flow);
      fset &= ~(fset - fcbn);

      moveset |= ((rset & RMASK[index]) | (fset & FMASK[index]))
         & game.occupancy[!side];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t zset;
   zset = (game.pieces[ps(side, 0x6)] << 9) >> (18 * side);
   zset &= ZMASK[side] & ~game.occupancy[side];
   add_shiftwise(zset, 9 - 18 * side, &moves);

   zset = game.pieces[ps(side, 0x6)] << 1 & FMASKN0;
   zset &= ZMASK[side] & ~game.occupancy[side];
   add_shiftwise(zset, 1, &moves);

   zset = game.pieces[ps(side, 0x6)] >> 1 & FMASKN8;
   zset &= ZMASK[side] & ~game.occupancy[side];
   add_shiftwise(zset, -1, &moves);

   __uint128_t xset = game.pieces[ps(side, 0x5)];
   while (xset) {
      uint64_t index = bsf_branchless(xset);
      xset = xset ^ PMASK[index];

      __uint128_t moveset;
      moveset = (PMASK[index] << 16 & game.pieces[empty] << 8)
         | (PMASK[index] << 20 & game.pieces[empty] << 10)
         | (PMASK[index] >> 16 & game.pieces[empty] >> 8)
         | (PMASK[index] >> 20 & game.pieces[empty] >> 10);
      moveset &= XMASK[side] & ~game.occupancy[side];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t sset = game.pieces[ps(side, 0x4)];
   while (sset) {
      uint64_t index = bsf_branchless(sset);
      sset = sset ^ PMASK[index];

      __uint128_t moveset;
      moveset = PMASK[index] << 8 | PMASK[index] << 10
         | PMASK[index] >> 8 | PMASK[index] >> 10;
      moveset &= SMASK[side] & ~game.occupancy[side];

      add_piecewise(moveset, index, &moves);
   }

   {
      __uint128_t moveset;
      moveset = game.pieces[ps(side, 0x0)] << 9
         | game.pieces[ps(side, 0x0)] >> 9
         | game.pieces[ps(side, 0x0)] << 1
         | game.pieces[ps(side, 0x0)] >> 1;
      moveset &= JMASK[side] & ~game.occupancy[side];
      add_piecewise(moveset, bsf_branchless(
         game.pieces[ps(side, 0x0)]), &moves);
   }

   return moves;
}

move_array_t generate_pseudolegal(int64_t side) {
   move_array_t moves = {malloc(111 * sizeof(move_t)), 0, 0};
   if (!game.pieces[ps(side, 0x0)]) { return moves; }

   __uint128_t C3U128 = 0x3;

   __uint128_t jset = game.pieces[ps(side, 0x1)];
   while (jset) {
      uint64_t index = bsf_branchless(jset);
      jset = jset ^ PMASK[index];

      __uint128_t rset, rlow, rcbn;
      __uint128_t fset, flow, fcbn;
      __uint128_t moveset;

      __uint128_t outer = (~game.pieces[empty] ^ PMASK[index]) | OMASK[index];

      rlow = outer & LMASK[index];
      rcbn = C3U128 << bsr_branchless(rlow);
      rset = outer ^ (outer - rcbn);

      flow = rlow & FMASK[index];
      fcbn = C3U128 << bsr_branchless(flow);
      fset = outer & FMASK[index];
      fset = fset ^ (fset - fcbn);

      moveset = (rset & RMASK[index]) | (fset & FMASK[index]);
      moveset &= ~game.occupancy[side];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t mset = game.pieces[ps(side, 0x2)];
   while (mset) {
      uint64_t index = bsf_branchless(mset);
      mset = mset ^ PMASK[index];

      __uint128_t moveset = 0;
      __uint128_t lset = PMASK[index] & FMASKN01 & (game.pieces[empty] << 1);
      moveset = moveset | lset << 7 | lset >> 11;
      __uint128_t rset = PMASK[index] & FMASKN78 & (game.pieces[empty] >> 1);
      moveset = moveset | rset << 11 | rset >> 7;
      __uint128_t flset = PMASK[index] & FMASKN0 & (game.pieces[empty] >> 9);
      moveset = moveset | flset << 17;
      __uint128_t frset = PMASK[index] & FMASKN8 & (game.pieces[empty] >> 9);
      moveset = moveset | frset << 19;
      __uint128_t blset = PMASK[index] & FMASKN0 & (game.pieces[empty] << 9);
      moveset = moveset | blset >> 19;
      __uint128_t brset = PMASK[index] & FMASKN8 & (game.pieces[empty] << 9);
      moveset = moveset | brset >> 17;
      moveset &= BMASK & ~game.occupancy[side];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t pset = game.pieces[ps(side, 0x3)];
   while (pset) {
      uint64_t index = bsf_branchless(pset);
      pset = pset ^ PMASK[index];

      __uint128_t rset, rlow, rcbn;
      __uint128_t fset, flow, fcbn;
      __uint128_t moveset;

      __uint128_t occupancy = (~game.pieces[empty] ^ PMASK[index]);
      __uint128_t outer = occupancy | OMASK[index];

      rlow = outer & LMASK[index];
      rcbn = C3U128 << bsr_branchless(rlow);
      rset = outer ^ (outer - rcbn);

      flow = rlow & FMASK[index];
      fcbn = C3U128 << bsr_branchless(flow);
      fset = outer & FMASK[index];
      fset = fset ^ (fset - fcbn);

      moveset = (rset & RMASK[index]) | (fset & FMASK[index]);
      moveset &= game.pieces[empty];

      rset = ~rset & occupancy;
      rlow = rset & LMASK[index];
      rcbn = C3U128 << bsr_branchless(rlow);
      rset &= ~(rset - rcbn);

      fset = (~fset & occupancy) & FMASK[index];
      flow = fset & LMASK[index];
      fcbn = C3U128 << bsr_branchless(flow);
      fset &= ~(fset - fcbn);

      moveset |= ((rset & RMASK[index]) | (fset & FMASK[index]))
         & game.occupancy[!side];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t zset;
   zset = (game.pieces[ps(side, 0x6)] << 9) >> (18 * side);
   zset &= ZMASK[side] & ~game.occupancy[side];
   add_shiftwise(zset, 9 - 18 * side, &moves);

   zset = game.pieces[ps(side, 0x6)] << 1 & FMASKN0;
   zset &= ZMASK[side] & ~game.occupancy[side];
   add_shiftwise(zset, 1, &moves);

   zset = game.pieces[ps(side, 0x6)] >> 1 & FMASKN8;
   zset &= ZMASK[side] & ~game.occupancy[side];
   add_shiftwise(zset, -1, &moves);

   __uint128_t xset = game.pieces[ps(side, 0x5)];
   while (xset) {
      uint64_t index = bsf_branchless(xset);
      xset = xset ^ PMASK[index];

      __uint128_t moveset;
      moveset = (PMASK[index] << 16 & game.pieces[empty] << 8)
         | (PMASK[index] << 20 & game.pieces[empty] << 10)
         | (PMASK[index] >> 16 & game.pieces[empty] >> 8)
         | (PMASK[index] >> 20 & game.pieces[empty] >> 10);
      moveset &= XMASK[side] & ~game.occupancy[side];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t sset = game.pieces[ps(side, 0x4)];
   while (sset) {
      uint64_t index = bsf_branchless(sset);
      sset = sset ^ PMASK[index];

      __uint128_t moveset;
      moveset = PMASK[index] << 8 | PMASK[index] << 10
         | PMASK[index] >> 8 | PMASK[index] >> 10;
      moveset &= SMASK[side] & ~game.occupancy[side];

      add_piecewise(moveset, index, &moves);
   }

   {
      uint64_t index = bsf_branchless(game.pieces[ps(side, 0x0)]);
      __uint128_t moveset;
      moveset = game.pieces[ps(side, 0x0)] << 9
         | game.pieces[ps(side, 0x0)] >> 9
         | game.pieces[ps(side, 0x0)] << 1
         | game.pieces[ps(side, 0x0)] >> 1;
      moveset &= JMASK[side] & ~game.occupancy[side];
      add_piecewise(moveset, index, &moves);

      __uint128_t fly = (game.pieces[ps(black, 0x0)] << 1)
         - game.pieces[ps(red, 0x0)];
      fly &= FMASK[index] & ~game.pieces[empty];
      fly ^= game.pieces[ps(red, 0x0)] | game.pieces[ps(black, 0x0)];
      if (!fly) { add_piecewise(game.pieces[po(side, 0x0)], index, &moves); }
   }

   return moves;
}

move_array_t generate_captures(int64_t side) {
   move_array_t moves = {malloc(40 * sizeof(move_t)), 0, 0};
   if (!game.pieces[ps(side, 0x0)]) { return moves; }

   __uint128_t C3U128 = 0x3;

   __uint128_t jset = game.pieces[ps(side, 0x1)];
   while (jset) {
      uint64_t index = bsf_branchless(jset);
      jset = jset ^ PMASK[index];

      __uint128_t rset, rlow, rcbn;
      __uint128_t fset, flow, fcbn;
      __uint128_t moveset;

      __uint128_t outer = (~game.pieces[empty] ^ PMASK[index]) | OMASK[index];

      rlow = outer & LMASK[index];
      rcbn = C3U128 << bsr_branchless(rlow);
      rset = outer ^ (outer - rcbn);

      flow = rlow & FMASK[index];
      fcbn = C3U128 << bsr_branchless(flow);
      fset = outer & FMASK[index];
      fset = fset ^ (fset - fcbn);

      moveset = (rset & RMASK[index]) | (fset & FMASK[index]);
      moveset &= game.occupancy[!side];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t mset = game.pieces[ps(side, 0x2)];
   while (mset) {
      uint64_t index = bsf_branchless(mset);
      mset = mset ^ PMASK[index];

      __uint128_t moveset = 0;
      __uint128_t lset = PMASK[index] & FMASKN01 & (game.pieces[empty] << 1);
      moveset = moveset | lset << 7 | lset >> 11;
      __uint128_t rset = PMASK[index] & FMASKN78 & (game.pieces[empty] >> 1);
      moveset = moveset | rset << 11 | rset >> 7;
      __uint128_t flset = PMASK[index] & FMASKN0 & (game.pieces[empty] >> 9);
      moveset = moveset | flset << 17;
      __uint128_t frset = PMASK[index] & FMASKN8 & (game.pieces[empty] >> 9);
      moveset = moveset | frset << 19;
      __uint128_t blset = PMASK[index] & FMASKN0 & (game.pieces[empty] << 9);
      moveset = moveset | blset >> 19;
      __uint128_t brset = PMASK[index] & FMASKN8 & (game.pieces[empty] << 9);
      moveset = moveset | brset >> 17;
      moveset &= BMASK & game.occupancy[!side];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t pset = game.pieces[ps(side, 0x3)];
   while (pset) {
      uint64_t index = bsf_branchless(pset);
      pset = pset ^ PMASK[index];

      __uint128_t rset, rlow, rcbn;
      __uint128_t fset, flow, fcbn;
      __uint128_t moveset;

      __uint128_t occupancy = (~game.pieces[empty] ^ PMASK[index]);
      __uint128_t outer = occupancy | OMASK[index];

      rlow = outer & LMASK[index];
      rcbn = C3U128 << bsr_branchless(rlow);
      rset = ~(outer ^ (outer - rcbn)) & occupancy;

      fset = outer & FMASK[index];
      flow = rlow & FMASK[index];
      fcbn = C3U128 << bsr_branchless(flow);
      fset = ~(fset ^ (fset - fcbn)) & occupancy;
      fset = fset & FMASK[index];
      flow = fset & LMASK[index];
      fcbn = C3U128 << bsr_branchless(flow);
      fset &= ~(fset - fcbn);

      rlow = rset & LMASK[index];
      rcbn = C3U128 << bsr_branchless(rlow);
      rset &= ~(rset - rcbn);

      moveset = ((rset & RMASK[index]) | (fset & FMASK[index]))
         & game.occupancy[!side];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t zset;
   zset = (game.pieces[ps(side, 0x6)] << 9) >> (18 * side);
   zset &= ZMASK[side] & game.occupancy[!side];
   add_shiftwise(zset, 9 - 18 * side, &moves);

   zset = game.pieces[ps(side, 0x6)] << 1 & FMASKN0;
   zset &= ZMASK[side] & game.occupancy[!side];
   add_shiftwise(zset, 1, &moves);

   zset = game.pieces[ps(side, 0x6)] >> 1 & FMASKN8;
   zset &= ZMASK[side] & game.occupancy[!side];
   add_shiftwise(zset, -1, &moves);

   __uint128_t xset = game.pieces[ps(side, 0x5)];
   while (xset) {
      uint64_t index = bsf_branchless(xset);
      xset = xset ^ PMASK[index];

      __uint128_t moveset;
      moveset = (PMASK[index] << 16 & game.pieces[empty] << 8)
         | (PMASK[index] << 20 & game.pieces[empty] << 10)
         | (PMASK[index] >> 16 & game.pieces[empty] >> 8)
         | (PMASK[index] >> 20 & game.pieces[empty] >> 10);
      moveset &= XMASK[side] & game.occupancy[!side];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t sset = game.pieces[ps(side, 0x4)];
   while (sset) {
      uint64_t index = bsf_branchless(sset);
      sset = sset ^ PMASK[index];

      __uint128_t moveset;
      moveset = PMASK[index] << 8 | PMASK[index] << 10
         | PMASK[index] >> 8 | PMASK[index] >> 10;
      moveset &= SMASK[side] & game.occupancy[!side];

      add_piecewise(moveset, index, &moves);
   }

   {
      uint64_t index = bsf_branchless(game.pieces[ps(side, 0x0)]);
      __uint128_t moveset;
      moveset = game.pieces[ps(side, 0x0)] << 9
         | game.pieces[ps(side, 0x0)] >> 9
         | game.pieces[ps(side, 0x0)] << 1
         | game.pieces[ps(side, 0x0)] >> 1;
      moveset &= JMASK[side] & game.occupancy[!side];
      add_piecewise(moveset, index, &moves);

      __uint128_t fly = (game.pieces[ps(black, 0x0)] << 1)
         - game.pieces[ps(red, 0x0)];
      fly &= FMASK[index] & ~game.pieces[empty];
      fly ^= game.pieces[ps(red, 0x0)] | game.pieces[ps(black, 0x0)];
      if (!fly) { add_piecewise(game.pieces[po(side, 0x0)], index, &moves); }
   }

   return moves;
}

move_array_t generate_quiet(int64_t side) {
   move_array_t moves = {malloc(111 * sizeof(move_t)), 0, 0};
   if (!game.pieces[ps(side, 0x0)]) { return moves; }

   __uint128_t C3U128 = 0x3;

   __uint128_t jpset = game.pieces[ps(side, 0x1)] | game.pieces[ps(side, 0x3)];
   while (jpset) {
      uint64_t index = bsf_branchless(jpset);
      jpset = jpset ^ PMASK[index];

      __uint128_t rset, rlow, rcbn;
      __uint128_t fset, flow, fcbn;
      __uint128_t moveset;

      __uint128_t outer = (~game.pieces[empty] ^ PMASK[index]) | OMASK[index];

      rlow = outer & LMASK[index];
      rcbn = C3U128 << bsr_branchless(rlow);
      rset = outer ^ (outer - rcbn);

      flow = rlow & FMASK[index];
      fcbn = C3U128 << bsr_branchless(flow);
      fset = outer & FMASK[index];
      fset = fset ^ (fset - fcbn);

      moveset = (rset & RMASK[index]) | (fset & FMASK[index]);
      moveset &= game.pieces[empty];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t mset = game.pieces[ps(side, 0x2)];
   while (mset) {
      uint64_t index = bsf_branchless(mset);
      mset = mset ^ PMASK[index];

      __uint128_t moveset = 0;
      __uint128_t lset = PMASK[index] & FMASKN01 & (game.pieces[empty] << 1);
      moveset = moveset | lset << 7 | lset >> 11;
      __uint128_t rset = PMASK[index] & FMASKN78 & (game.pieces[empty] >> 1);
      moveset = moveset | rset << 11 | rset >> 7;
      __uint128_t flset = PMASK[index] & FMASKN0 & (game.pieces[empty] >> 9);
      moveset = moveset | flset << 17;
      __uint128_t frset = PMASK[index] & FMASKN8 & (game.pieces[empty] >> 9);
      moveset = moveset | frset << 19;
      __uint128_t blset = PMASK[index] & FMASKN0 & (game.pieces[empty] << 9);
      moveset = moveset | blset >> 19;
      __uint128_t brset = PMASK[index] & FMASKN8 & (game.pieces[empty] << 9);
      moveset = moveset | brset >> 17;
      moveset &= BMASK & game.pieces[empty];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t zset;
   zset = (game.pieces[ps(side, 0x6)] << 9) >> (18 * side);
   zset &= ZMASK[side] & game.pieces[empty];
   add_shiftwise(zset, 9 - 18 * side, &moves);

   zset = game.pieces[ps(side, 0x6)] << 1 & FMASKN0;
   zset &= ZMASK[side] & game.pieces[empty];
   add_shiftwise(zset, 1, &moves);

   zset = game.pieces[ps(side, 0x6)] >> 1 & FMASKN8;
   zset &= ZMASK[side] & game.pieces[empty];
   add_shiftwise(zset, -1, &moves);

   __uint128_t xset = game.pieces[ps(side, 0x5)];
   while (xset) {
      uint64_t index = bsf_branchless(xset);
      xset = xset ^ PMASK[index];

      __uint128_t moveset;
      moveset = (PMASK[index] << 16 & game.pieces[empty] << 8)
         | (PMASK[index] << 20 & game.pieces[empty] << 10)
         | (PMASK[index] >> 16 & game.pieces[empty] >> 8)
         | (PMASK[index] >> 20 & game.pieces[empty] >> 10);
      moveset &= XMASK[side] & game.pieces[empty];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t sset = game.pieces[ps(side, 0x4)];
   while (sset) {
      uint64_t index = bsf_branchless(sset);
      sset = sset ^ PMASK[index];

      __uint128_t moveset;
      moveset = PMASK[index] << 8 | PMASK[index] << 10
         | PMASK[index] >> 8 | PMASK[index] >> 10;
      moveset &= SMASK[side] & game.pieces[empty];

      add_piecewise(moveset, index, &moves);
   }

   {
      __uint128_t moveset;
      moveset = game.pieces[ps(side, 0x0)] << 9
         | game.pieces[ps(side, 0x0)] >> 9
         | game.pieces[ps(side, 0x0)] << 1
         | game.pieces[ps(side, 0x0)] >> 1;
      moveset &= JMASK[side] & game.pieces[empty];
      add_piecewise(moveset, bsf_branchless(
         game.pieces[ps(side, 0x0)]), &moves);
   }

   return moves;
}

void add_piecewise(__uint128_t set, uint64_t from, move_array_t* moves) {
   for (; set; set &= set - 1) {
      uint64_t to = bsf_branchless(set);
      move_t move = { ._ = { from, to, board[from], board[to] } };
      moves->data[moves->count++] = move;
   }
}

void add_shiftwise(__uint128_t set, int64_t shift, move_array_t* moves) {
   for (; set; set &= set - 1) {
      uint64_t to = bsf_branchless(set);
      uint64_t from = to - shift;
      move_t move = { ._ = { from, to, board[from], board[to] } };
      moves->data[moves->count++] = move;
   }
}

move_array_t sort_moves(move_array_t moves) {
   move_array_t sorted = {
      malloc(moves.count * sizeof(move_t)), moves.count, 0
   };

   int32_t indices[9] = {0};
   int32_t* counts = &indices[1];
   for (int64_t i = 0; i != moves.count; ++i)
      ++counts[p(moves.data[i]._.pto)];
   for (int64_t i = 1; i != 7; ++i)
      counts[i] = counts[i] + counts[i - 1];
   for (int64_t i = 0; i != moves.count; ++i)
      sorted.data[indices[p(moves.data[i]._.pto)]++] = moves.data[i];
   sorted.quiet = indices[7];

   free(moves.data);

   return sorted;
}

move_t next(generator_t* engine) {
   switch (engine->state) {
      case 0:
         ++(engine->state);
         if (engine->move.bits)
            return engine->move;
      case 1:
         ++(engine->state);
         engine->moves = sort_moves(generate_pseudolegal(state.side));
      case 2:
         if (engine->index < engine->moves.quiet)
            return engine->moves.data[engine->index++];
      case 3:
         ++(engine->state);
         ++(engine->state);
         move_t killer = ktable[state.ply][0].move;
         if (killer.bits && is_valid(killer, state.side))
            return killer;
      case 4:
         ++(engine->state);
         killer = ktable[state.ply][1].move;
         if (killer.bits && is_valid(killer, state.side))
            return killer;
      case 5:
         ++(engine->state);
      case 6:
         if (engine->index < engine->moves.count)
            return engine->moves.data[engine->index++];
      default:
         return (move_t){0};
         break;
   }
}
