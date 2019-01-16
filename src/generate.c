#include "generate.h"

#include "inlines.h"
#include "magics.h"
#include "masks.h"
#include "state.h"

#include <stdlib.h>

move_array_t generate(uint32_t side) {
   move_array_t moves = {malloc(111 * sizeof(move_t)), 0, 0};

   __uint128_t C3U128 = 0x3;

   __uint128_t jset = game.pieces[ps(side, 0x1)];
   for (; jset; jset &= jset - 1) {
      uint32_t index = bsf_branchless(jset);

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
   for (; mset; mset &= mset - 1) {
      uint32_t index = bsf_branchless(mset);

      __uint128_t moveset;
      moveset = (PMASK[index] << 0x11 & game.pieces[empty] << 0x8 & FMASKN8)
         | (PMASK[index] >> 0x11 & game.pieces[empty] >> 0x8 & FMASKN0)
         | (PMASK[index] << 0x13 & game.pieces[empty] << 0xa & FMASKN0)
         | (PMASK[index] >> 0x13 & game.pieces[empty] >> 0xa & FMASKN8)
         | (PMASK[index] << 0x07 & game.pieces[empty] << 0x8 & FMASKN78)
         | (PMASK[index] >> 0x07 & game.pieces[empty] >> 0x8 & FMASKN01)
         | (PMASK[index] << 0x0b & game.pieces[empty] << 0xa & FMASKN01)
         | (PMASK[index] >> 0x0b & game.pieces[empty] >> 0xa & FMASKN78);
      moveset &= BMASK & ~game.occupancy[side];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t pset = game.pieces[ps(side, 0x3)];
   for (; pset; pset &= pset - 1) {
      uint32_t index = bsf_branchless(pset);

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
   for (; xset; xset &= xset - 1) {
      uint32_t index = bsf_branchless(xset);

      __uint128_t moveset;
      moveset = (PMASK[index] << 0x10 & game.pieces[empty] << 0x8)
         | (PMASK[index] << 0x14 & game.pieces[empty] << 0xa)
         | (PMASK[index] >> 0x10 & game.pieces[empty] >> 0x8)
         | (PMASK[index] >> 0x14 & game.pieces[empty] >> 0xa);
      moveset &= XMASK[side] & ~game.occupancy[side];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t sset = game.pieces[ps(side, 0x4)];
   for (; sset; sset &= sset - 1) {
      uint32_t index = bsf_branchless(sset);

      __uint128_t moveset;
      moveset = PMASK[index] << 0x8 | PMASK[index] << 0xa
         | PMASK[index] >> 0x8 | PMASK[index] >> 0xa;
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

move_array_t generate_pseudolegal(uint32_t side) {
   move_array_t moves = {malloc(111 * sizeof(move_t)), 0, 0};
   if (!game.pieces[ps(side, 0x0)]) { return moves; }

   __uint128_t C3U128 = 0x3;

   __uint128_t jset = game.pieces[ps(side, 0x1)];
   for (; jset; jset &= jset - 1) {
      uint32_t index = bsf_branchless(jset);

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
   for (; mset; mset &= mset - 1) {
      uint32_t index = bsf_branchless(mset);

      __uint128_t moveset;
      moveset = (PMASK[index] << 0x11 & game.pieces[empty] << 0x8 & FMASKN8)
         | (PMASK[index] >> 0x11 & game.pieces[empty] >> 0x8 & FMASKN0)
         | (PMASK[index] << 0x13 & game.pieces[empty] << 0xa & FMASKN0)
         | (PMASK[index] >> 0x13 & game.pieces[empty] >> 0xa & FMASKN8)
         | (PMASK[index] << 0x07 & game.pieces[empty] << 0x8 & FMASKN78)
         | (PMASK[index] >> 0x07 & game.pieces[empty] >> 0x8 & FMASKN01)
         | (PMASK[index] << 0x0b & game.pieces[empty] << 0xa & FMASKN01)
         | (PMASK[index] >> 0x0b & game.pieces[empty] >> 0xa & FMASKN78);
      moveset &= BMASK & ~game.occupancy[side];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t pset = game.pieces[ps(side, 0x3)];
   for (; pset; pset &= pset - 1) {
      uint32_t index = bsf_branchless(pset);

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
   for (; xset; xset &= xset - 1) {
      uint32_t index = bsf_branchless(xset);

      __uint128_t moveset;
      moveset = (PMASK[index] << 0x10 & game.pieces[empty] << 0x8)
         | (PMASK[index] << 0x14 & game.pieces[empty] << 0xa)
         | (PMASK[index] >> 0x10 & game.pieces[empty] >> 0x8)
         | (PMASK[index] >> 0x14 & game.pieces[empty] >> 0xa);
      moveset &= XMASK[side] & ~game.occupancy[side];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t sset = game.pieces[ps(side, 0x4)];
   for (; sset; sset &= sset - 1) {
      uint32_t index = bsf_branchless(sset);

      __uint128_t moveset;
      moveset = PMASK[index] << 0x8 | PMASK[index] << 0xa
         | PMASK[index] >> 0x8 | PMASK[index] >> 0xa;
      moveset &= SMASK[side] & ~game.occupancy[side];

      add_piecewise(moveset, index, &moves);
   }

   {
      uint32_t index = bsf_branchless(game.pieces[ps(side, 0x0)]);
      __uint128_t moveset;
      moveset = game.pieces[ps(side, 0x0)] << 9
         | game.pieces[ps(side, 0x0)] >> 9
         | game.pieces[ps(side, 0x0)] << 1
         | game.pieces[ps(side, 0x0)] >> 1;
      moveset &= JMASK[side] & ~game.occupancy[side];
      add_piecewise(moveset, index, &moves);

      __uint128_t fly = (game.pieces[po(0x0, 0x0)] << 1)
         - game.pieces[ps(0x0, 0x0)];
      fly &= FMASK[index] & ~game.pieces[empty];
      fly ^= game.pieces[ps(0x0, 0x0)] | game.pieces[po(0x0, 0x0)];
      if (!fly) { add_piecewise(game.pieces[po(side, 0x0)], index, &moves); }
   }

   return moves;
}

move_array_t generate_captures(uint32_t side) {
   move_array_t moves = {malloc(40 * sizeof(move_t)), 0, 0};
   if (!game.pieces[ps(side, 0x0)]) { return moves; }

   __uint128_t C3U128 = 0x3;

   __uint128_t jset = game.pieces[ps(side, 0x1)];
   for (; jset; jset &= jset - 1) {
      uint32_t index = bsf_branchless(jset);

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
   for (; mset; mset &= mset - 1) {
      uint32_t index = bsf_branchless(mset);

      __uint128_t moveset;
      moveset = (PMASK[index] << 0x11 & game.pieces[empty] << 0x8 & FMASKN8)
         | (PMASK[index] >> 0x11 & game.pieces[empty] >> 0x8 & FMASKN0)
         | (PMASK[index] << 0x13 & game.pieces[empty] << 0xa & FMASKN0)
         | (PMASK[index] >> 0x13 & game.pieces[empty] >> 0xa & FMASKN8)
         | (PMASK[index] << 0x07 & game.pieces[empty] << 0x8 & FMASKN78)
         | (PMASK[index] >> 0x07 & game.pieces[empty] >> 0x8 & FMASKN01)
         | (PMASK[index] << 0x0b & game.pieces[empty] << 0xa & FMASKN01)
         | (PMASK[index] >> 0x0b & game.pieces[empty] >> 0xa & FMASKN78);
      moveset &= BMASK & game.occupancy[!side];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t pset = game.pieces[ps(side, 0x3)];
   for (; pset; pset &= pset - 1) {
      uint32_t index = bsf_branchless(pset);

      __uint128_t rset, rlow, rcbn;
      __uint128_t fset, flow, fcbn;
      __uint128_t moveset;

      __uint128_t occupancy = (~game.pieces[empty] ^ PMASK[index]);
      __uint128_t outer = occupancy | OMASK[index];

      rlow = outer & LMASK[index];

      flow = rlow & FMASK[index];

      rcbn = C3U128 << bsr_branchless(rlow);
      rset = outer ^ (outer - rcbn);
      rset = ~rset & occupancy;
      rlow = rset & LMASK[index];
      rcbn = C3U128 << bsr_branchless(rlow);
      rset &= ~(rset - rcbn);

      fcbn = C3U128 << bsr_branchless(flow);
      fset = outer & FMASK[index];
      fset = fset ^ (fset - fcbn);
      fset = (~fset & occupancy) & FMASK[index];
      flow = fset & LMASK[index];
      fcbn = C3U128 << bsr_branchless(flow);
      fset &= ~(fset - fcbn);

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
   for (; xset; xset &= xset - 1) {
      uint32_t index = bsf_branchless(xset);

      __uint128_t moveset;
      moveset = (PMASK[index] << 0x10 & game.pieces[empty] << 0x8)
         | (PMASK[index] << 0x14 & game.pieces[empty] << 0xa)
         | (PMASK[index] >> 0x10 & game.pieces[empty] >> 0x8)
         | (PMASK[index] >> 0x14 & game.pieces[empty] >> 0xa);
      moveset &= XMASK[side] & game.occupancy[!side];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t sset = game.pieces[ps(side, 0x4)];
   for (; sset; sset &= sset - 1) {
      uint32_t index = bsf_branchless(sset);

      __uint128_t moveset;
      moveset = PMASK[index] << 0x8 | PMASK[index] << 0xa
         | PMASK[index] >> 0x8 | PMASK[index] >> 0xa;
      moveset &= SMASK[side] & game.occupancy[!side];

      add_piecewise(moveset, index, &moves);
   }

   {
      uint32_t index = bsf_branchless(game.pieces[ps(side, 0x0)]);
      __uint128_t moveset;
      moveset = game.pieces[ps(side, 0x0)] << 9
         | game.pieces[ps(side, 0x0)] >> 9
         | game.pieces[ps(side, 0x0)] << 1
         | game.pieces[ps(side, 0x0)] >> 1;
      moveset &= JMASK[side] & game.occupancy[!side];
      add_piecewise(moveset, index, &moves);

      __uint128_t fly = (game.pieces[po(0x0, 0x0)] << 1)
         - game.pieces[ps(0x0, 0x0)];
      fly &= FMASK[index] & ~game.pieces[empty];
      fly ^= game.pieces[ps(0x0, 0x0)] | game.pieces[po(0x0, 0x0)];
      if (!fly) { add_piecewise(game.pieces[po(side, 0x0)], index, &moves); }
   }

   return moves;
}

move_array_t generate_quiet(uint32_t side) {
   move_array_t moves = {malloc(111 * sizeof(move_t)), 0, 0};
   if (!game.pieces[ps(side, 0x0)]) { return moves; }

   __uint128_t C3U128 = 0x3;

   __uint128_t jpset = game.pieces[ps(side, 0x1)] | game.pieces[ps(side, 0x3)];
   for (; jpset; jpset &= jpset - 1) {
      uint32_t index = bsf_branchless(jpset);

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
   for (; mset; mset &= mset - 1) {
      uint32_t index = bsf_branchless(mset);

      __uint128_t moveset;
      moveset = (PMASK[index] << 0x11 & game.pieces[empty] << 0x8 & FMASKN8)
         | (PMASK[index] >> 0x11 & game.pieces[empty] >> 0x8 & FMASKN0)
         | (PMASK[index] << 0x13 & game.pieces[empty] << 0xa & FMASKN0)
         | (PMASK[index] >> 0x13 & game.pieces[empty] >> 0xa & FMASKN8)
         | (PMASK[index] << 0x07 & game.pieces[empty] << 0x8 & FMASKN78)
         | (PMASK[index] >> 0x07 & game.pieces[empty] >> 0x8 & FMASKN01)
         | (PMASK[index] << 0x0b & game.pieces[empty] << 0xa & FMASKN01)
         | (PMASK[index] >> 0x0b & game.pieces[empty] >> 0xa & FMASKN78);
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
   for (; xset; xset &= xset - 1) {
      uint32_t index = bsf_branchless(xset);

      __uint128_t moveset;
      moveset = (PMASK[index] << 0x10 & game.pieces[empty] << 0x8)
         | (PMASK[index] << 0x14 & game.pieces[empty] << 0xa)
         | (PMASK[index] >> 0x10 & game.pieces[empty] >> 0x8)
         | (PMASK[index] >> 0x14 & game.pieces[empty] >> 0xa);
      moveset &= XMASK[side] & game.pieces[empty];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t sset = game.pieces[ps(side, 0x4)];
   for (; sset; sset &= sset - 1) {
      uint32_t index = bsf_branchless(sset);

      __uint128_t moveset;
      moveset = PMASK[index] << 0x8 | PMASK[index] << 0xa
         | PMASK[index] >> 0x8 | PMASK[index] >> 0xa;
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

void add_piecewise(__uint128_t set, uint32_t from, move_array_t* moves) {
   for (; set; set &= set - 1) {
      uint8_t to = bsf_branchless(set);
      move_t move = { ._ = { from, to, board[from], board[to] } };
      moves->data[moves->count++] = move;
   }
}

void add_shiftwise(__uint128_t set, int32_t shift, move_array_t* moves) {
   for (; set; set &= set - 1) {
      uint8_t to = bsf_branchless(set);
      uint8_t from = to - shift;
      move_t move = { ._ = { from, to, board[from], board[to] } };
      moves->data[moves->count++] = move;
   }
}

uint32_t in_check(uint32_t side) {
   uint32_t index = bsf_branchless(game.pieces[ps(side, 0x0)]);

   __uint128_t jset = game.pieces[po(side, 0x0)] | game.pieces[po(side, 0x1)];
   __uint128_t jrset = jset & RMASK[index];
   __uint128_t jfset = jset & FMASK[index];

   __uint128_t xmask = PMASK[index] | game.pieces[empty];
   __uint128_t jxmask = ~(jset | xmask);

   for (; jrset; jrset &= jrset - 1) {
      __uint128_t range = jrset ^ -jrset ^ UMASK[index];
      range = range & jxmask & RMASK[index];
      if (!range) { return 1; }
   }

   for (; jfset; jfset &= jfset - 1) {
      __uint128_t range = jfset ^ -jfset ^ UMASK[index];
      range = range & jxmask & FMASK[index];
      if (!range) { return 1; }
   }

   __uint128_t pset = game.pieces[po(side, 0x3)];
   __uint128_t prset = pset & RMASK[index];
   __uint128_t pfset = pset & FMASK[index];

   for (; prset; prset &= prset - 1) {
      __uint128_t range = prset ^ -prset ^ UMASK[index];
      __uint128_t lsb = prset & -prset;
      __uint128_t pxmask = ~(lsb | xmask);
      range = range & pxmask & RMASK[index];
      if (popcnt(range) == 1) { return 1; }
   }

   for (; pfset; pfset &= pfset - 1) {
      __uint128_t range = pfset ^ -pfset ^ UMASK[index];
      __uint128_t lsb = pfset & -pfset;
      __uint128_t pxmask = ~(lsb | xmask);
      range = range & pxmask & FMASK[index];
      if (popcnt(range) == 1) { return 1; }
   }

   __uint128_t mset = (PMASK[index] << 0x11 & game.pieces[empty] << 0x9)
      | (PMASK[index] >> 0x11 & game.pieces[empty] >> 0x9)
      | (PMASK[index] << 0x13 & game.pieces[empty] << 0x9)
      | (PMASK[index] >> 0x13 & game.pieces[empty] >> 0x9)
      | (PMASK[index] << 0x07 & game.pieces[empty] >> 0x1)
      | (PMASK[index] >> 0x07 & game.pieces[empty] << 0x1)
      | (PMASK[index] << 0x0b & game.pieces[empty] << 0x1)
      | (PMASK[index] >> 0x0b & game.pieces[empty] >> 0x1);
   mset = mset & game.pieces[po(side, 0x2)];
   if (mset) { return 1; }

   __uint128_t zset = (PMASK[index] << 9) >> (18 * side)
      | PMASK[index] << 1 | PMASK[index] >> 1;
   zset = zset & game.pieces[po(side, 0x6)];
   if (zset) { return 1; }

   return 0;
}

uint32_t is_legal(move_t move, uint32_t side) {
   if (side != s(move._.pfrom)) { return 0; }

   uint32_t from = move._.from;
   if (board[from] != move._.pfrom) { return 0; }

   uint32_t to = move._.to;
   if (board[to] != move._.pto) { return 0; }

   switch (p(move._.pfrom)) {
      case 0: {
         if (p(move._.pto)) { return 1; }
         __uint128_t jspan = game.pieces[po(0x0, 0x0)]
            - (game.pieces[ps(0x0, 0x0)] << 1);
         jspan &= FMASK[from] & ~game.pieces[empty];
         return !jspan; }
      case 1: {
         uint32_t high = max(from, to);
         uint32_t low = min(from, to);

         __uint128_t jspan = PMASK[high] - (PMASK[low] << 1);
         if (high - low > 8) { jspan &= FMASK[high]; }
         jspan &= ~game.pieces[empty];
         return !jspan; }
      case 2:
         switch (from - to) {
            case -19: case -11: { return board[to - 10] == empty; }
            case -17: case -7: { return board[to - 8] == empty; }
            case 7: case 17: { return board[to + 8] == empty; }
            case 11: case 19: { return board[to + 10] == empty; }
            default: { return 0; }
         }
      case 3: {
         uint32_t high = max(from, to);
         uint32_t low = min(from, to);

         __uint128_t pspan = PMASK[high] - (PMASK[low] << 1);
         if (high - low > 8) { pspan &= FMASK[high]; }
         pspan &= ~game.pieces[empty];

         if (move._.pto == empty) { return !pspan; }
         else { return popcnt(pspan) == 1; } }
      case 4: return 1;
      case 5: return board[(from + to) / 2] == empty;
      case 6: return 1;
      default: return 0;
   }
}

move_array_t sort_moves(move_array_t moves) {
   move_array_t sorted = {
      malloc(moves.count * sizeof(move_t)), moves.count, 0
   };

   uint32_t indices[9] = {0};
   uint32_t* counts = &indices[1];
   for (uint32_t i = 0; i < moves.count; ++i)
      ++counts[p(moves.data[i]._.pto)];
   for (uint32_t i = 1; i < 7; ++i)
      counts[i] = counts[i] + counts[i - 1];
   for (uint32_t i = 0; i < moves.count; ++i)
      sorted.data[indices[p(moves.data[i]._.pto)]++] = moves.data[i];
   sorted.quiet = indices[7];

   free(moves.data);

   return sorted;
}
