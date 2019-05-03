#include "generate.h"

#include "inlines.h"
#include "magics.h"
#include "masks.h"
#include "memory.h"
#include "position.h"
#include "state.h"

#include <stdlib.h>
#include <string.h>

move_array_t generate(int64_t side) {
   move_array_t moves = {malloc(111 * sizeof(move_t)), 0, 0};

   __uint128_t C3U128 = 0x3;

   __uint128_t jset = game.pieces[ps(side, 0x1)];
   while (jset) {
      uint64_t index = bsf(jset);
      jset = jset ^ PMASK[index];

      __uint128_t rset, rlow, rcbn;
      __uint128_t fset, flow, fcbn;
      __uint128_t moveset;

      __uint128_t outer = (~game.pieces[empty] ^ PMASK[index]) | OMASK[index];

      rlow = outer & LMASK[index];
      rcbn = C3U128 << bsr(rlow);
      rset = outer ^ (outer - rcbn);

      flow = rlow & FMASK[index];
      fcbn = C3U128 << bsr(flow);
      fset = outer & FMASK[index];
      fset = fset ^ (fset - fcbn);

      moveset = (rset & RMASK[index]) | (fset & FMASK[index]);
      moveset = moveset & ~game.occupancy[side];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t mset = game.pieces[ps(side, 0x2)];
   while (mset) {
      uint64_t index = bsf(mset);
      mset = mset ^ PMASK[index];

      __uint128_t moveset = 0;
      __uint128_t lset = PMASK[index] & _1e(game.pieces[empty]);
      moveset = moveset | _2w1n(lset) | _2w1s(lset);
      __uint128_t rset = PMASK[index] & _1w(game.pieces[empty]);
      moveset = moveset | _2e1n(rset) | _2e1s(rset);
      __uint128_t flset = PMASK[index] & _1s(game.pieces[empty]);
      moveset = moveset | _2n1w(flset);
      __uint128_t frset = PMASK[index] & _1s(game.pieces[empty]);
      moveset = moveset | _2n1e(frset);
      __uint128_t blset = PMASK[index] & _1n(game.pieces[empty]);
      moveset = moveset | _2s1w(blset);
      __uint128_t brset = PMASK[index] & _1n(game.pieces[empty]);
      moveset = moveset | _2s1e(brset);
      moveset = moveset & BMASK & ~game.occupancy[side];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t pset = game.pieces[ps(side, 0x3)];
   while (pset) {
      uint64_t index = bsf(pset);
      pset = pset ^ PMASK[index];

      __uint128_t rset, rlow, rcbn;
      __uint128_t fset, flow, fcbn;
      __uint128_t moveset, captures;

      __uint128_t occupancy = (~game.pieces[empty] ^ PMASK[index]);
      __uint128_t outer = occupancy | OMASK[index];

      rlow = outer & LMASK[index];
      rcbn = C3U128 << bsr(rlow);
      rset = outer ^ (outer - rcbn);

      flow = rlow & FMASK[index];
      fcbn = C3U128 << bsr(flow);
      fset = outer & FMASK[index];
      fset = fset ^ (fset - fcbn);

      moveset = (rset & RMASK[index]) | (fset & FMASK[index]);
      moveset = moveset & game.pieces[empty];

      rset = ~rset & occupancy;
      rlow = rset & LMASK[index];
      rcbn = C3U128 << bsr(rlow);
      rset = rset & ~(rset - rcbn);

      fset = (~fset & occupancy) & FMASK[index];
      flow = fset & LMASK[index];
      fcbn = C3U128 << bsr(flow);
      fset = fset & ~(fset - fcbn);

      captures = (rset & RMASK[index]) | (fset & FMASK[index]);
      captures = captures & game.occupancy[!side];
      moveset = moveset | captures;

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t zset;
   zset = _1f(game.pieces[ps(side, 0x6)], side);
   zset &= ZMASK[side] & ~game.occupancy[side];
   add_shiftwise(zset, side ? -WIDTH : WIDTH, &moves);

   zset = _1e(game.pieces[ps(side, 0x6)]) & BMASK;
   zset &= ZMASK[side] & ~game.occupancy[side];
   add_shiftwise(zset, 1, &moves);

   zset = _1w(game.pieces[ps(side, 0x6)]) & BMASK;
   zset &= ZMASK[side] & ~game.occupancy[side];
   add_shiftwise(zset, -1, &moves);

   __uint128_t xset = game.pieces[ps(side, 0x5)];
   while (xset) {
      uint64_t index = bsf(xset);
      xset = xset ^ PMASK[index];

      __uint128_t moveset;
      moveset = (_2n2w(PMASK[index]) & _1n1w(game.pieces[empty]))
         | (_2n2e(PMASK[index]) & _1n1e(game.pieces[empty]))
         | (_2s2e(PMASK[index]) & _1s1e(game.pieces[empty]))
         | (_2s2w(PMASK[index]) & _1s1w(game.pieces[empty]));
      moveset = moveset & XMASK[side] & ~game.occupancy[side];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t sset = game.pieces[ps(side, 0x4)];
   while (sset) {
      uint64_t index = bsf(sset);
      sset = sset ^ PMASK[index];

      __uint128_t moveset;
      moveset = _1n1w(PMASK[index]) | _1n1e(PMASK[index])
         | _1s1e(PMASK[index]) | _1s1w(PMASK[index]);
      moveset = moveset & SMASK[side] & ~game.occupancy[side];

      add_piecewise(moveset, index, &moves);
   }

   {
      __uint128_t moveset;
      moveset = _1n(game.pieces[ps(side, 0x0)])
         | _1s(game.pieces[ps(side, 0x0)])
         | _1e(game.pieces[ps(side, 0x0)])
         | _1w(game.pieces[ps(side, 0x0)]);
      moveset = moveset & JMASK[side] & ~game.occupancy[side];
      add_piecewise(moveset, bsf(
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
      uint64_t index = bsf(jset);
      jset = jset ^ PMASK[index];

      __uint128_t rset, rlow, rcbn;
      __uint128_t fset, flow, fcbn;
      __uint128_t moveset;

      __uint128_t outer = (~game.pieces[empty] ^ PMASK[index]) | OMASK[index];

      rlow = outer & LMASK[index];
      rcbn = C3U128 << bsr(rlow);
      rset = outer ^ (outer - rcbn);

      flow = rlow & FMASK[index];
      fcbn = C3U128 << bsr(flow);
      fset = outer & FMASK[index];
      fset = fset ^ (fset - fcbn);

      moveset = (rset & RMASK[index]) | (fset & FMASK[index]);
      moveset = moveset & ~game.occupancy[side];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t mset = game.pieces[ps(side, 0x2)];
   while (mset) {
      uint64_t index = bsf(mset);
      mset = mset ^ PMASK[index];

      __uint128_t moveset = 0;
      __uint128_t lset = PMASK[index] & _1e(game.pieces[empty]);
      moveset = moveset | _2w1n(lset) | _2w1s(lset);
      __uint128_t rset = PMASK[index] & _1w(game.pieces[empty]);
      moveset = moveset | _2e1n(rset) | _2e1s(rset);
      __uint128_t flset = PMASK[index] & _1s(game.pieces[empty]);
      moveset = moveset | _2n1w(flset);
      __uint128_t frset = PMASK[index] & _1s(game.pieces[empty]);
      moveset = moveset | _2n1e(frset);
      __uint128_t blset = PMASK[index] & _1n(game.pieces[empty]);
      moveset = moveset | _2s1w(blset);
      __uint128_t brset = PMASK[index] & _1n(game.pieces[empty]);
      moveset = moveset | _2s1e(brset);
      moveset = moveset & BMASK & ~game.occupancy[side];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t pset = game.pieces[ps(side, 0x3)];
   while (pset) {
      uint64_t index = bsf(pset);
      pset = pset ^ PMASK[index];

      __uint128_t rset, rlow, rcbn;
      __uint128_t fset, flow, fcbn;
      __uint128_t moveset, captures;

      __uint128_t occupancy = (~game.pieces[empty] ^ PMASK[index]);
      __uint128_t outer = occupancy | OMASK[index];

      rlow = outer & LMASK[index];
      rcbn = C3U128 << bsr(rlow);
      rset = outer ^ (outer - rcbn);

      flow = rlow & FMASK[index];
      fcbn = C3U128 << bsr(flow);
      fset = outer & FMASK[index];
      fset = fset ^ (fset - fcbn);

      moveset = (rset & RMASK[index]) | (fset & FMASK[index]);
      moveset = moveset & game.pieces[empty];

      rset = ~rset & occupancy;
      rlow = rset & LMASK[index];
      rcbn = C3U128 << bsr(rlow);
      rset = rset & ~(rset - rcbn);

      fset = (~fset & occupancy) & FMASK[index];
      flow = fset & LMASK[index];
      fcbn = C3U128 << bsr(flow);
      fset = fset & ~(fset - fcbn);

      captures = (rset & RMASK[index]) | (fset & FMASK[index]);
      captures = captures & game.occupancy[!side];
      moveset = moveset | captures;

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t zset;
   zset = _1f(game.pieces[ps(side, 0x6)], side);
   zset &= ZMASK[side] & ~game.occupancy[side];
   add_shiftwise(zset, side ? -WIDTH : WIDTH, &moves);

   zset = _1e(game.pieces[ps(side, 0x6)]) & BMASK;
   zset &= ZMASK[side] & ~game.occupancy[side];
   add_shiftwise(zset, 1, &moves);

   zset = _1w(game.pieces[ps(side, 0x6)]) & BMASK;
   zset &= ZMASK[side] & ~game.occupancy[side];
   add_shiftwise(zset, -1, &moves);

   __uint128_t xset = game.pieces[ps(side, 0x5)];
   while (xset) {
      uint64_t index = bsf(xset);
      xset = xset ^ PMASK[index];

      __uint128_t moveset;
      moveset = (_2n2w(PMASK[index]) & _1n1w(game.pieces[empty]))
         | (_2n2e(PMASK[index]) & _1n1e(game.pieces[empty]))
         | (_2s2e(PMASK[index]) & _1s1e(game.pieces[empty]))
         | (_2s2w(PMASK[index]) & _1s1w(game.pieces[empty]));
      moveset = moveset & XMASK[side] & ~game.occupancy[side];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t sset = game.pieces[ps(side, 0x4)];
   while (sset) {
      uint64_t index = bsf(sset);
      sset = sset ^ PMASK[index];

      __uint128_t moveset;
      moveset = _1n1w(PMASK[index]) | _1n1e(PMASK[index])
         | _1s1e(PMASK[index]) | _1s1w(PMASK[index]);
      moveset = moveset & SMASK[side] & ~game.occupancy[side];

      add_piecewise(moveset, index, &moves);
   }

   {
      uint64_t index = bsf(game.pieces[ps(side, 0x0)]);
      __uint128_t moveset;
      moveset = _1n(game.pieces[ps(side, 0x0)])
         | _1s(game.pieces[ps(side, 0x0)])
         | _1e(game.pieces[ps(side, 0x0)])
         | _1w(game.pieces[ps(side, 0x0)]);
      moveset = moveset & JMASK[side] & ~game.occupancy[side];

      __uint128_t fly = (game.pieces[ps(black, 0x0)] << 1)
         - game.pieces[ps(red, 0x0)];
      fly = fly & FMASK[index] & ~game.pieces[empty];
      fly ^= game.pieces[ps(red, 0x0)] | game.pieces[ps(black, 0x0)];

      moveset = moveset | (fly ? 0 : game.pieces[po(side, 0x0)]);
      add_piecewise(moveset, index, &moves);
   }

   return moves;
}

move_array_t generate_captures(int64_t side) {
   move_array_t moves = {malloc(40 * sizeof(move_t)), 0, 0};
   if (!game.pieces[ps(side, 0x0)]) { return moves; }

   __uint128_t C3U128 = 0x3;

   __uint128_t jset = game.pieces[ps(side, 0x1)];
   while (jset) {
      uint64_t index = bsf(jset);
      jset = jset ^ PMASK[index];

      __uint128_t rset, rlow, rcbn;
      __uint128_t fset, flow, fcbn;
      __uint128_t moveset;

      __uint128_t outer = (~game.pieces[empty] ^ PMASK[index]) | OMASK[index];

      rlow = outer & LMASK[index];
      rcbn = C3U128 << bsr(rlow);
      rset = outer ^ (outer - rcbn);

      flow = rlow & FMASK[index];
      fcbn = C3U128 << bsr(flow);
      fset = outer & FMASK[index];
      fset = fset ^ (fset - fcbn);

      moveset = (rset & RMASK[index]) | (fset & FMASK[index]);
      moveset = moveset & game.occupancy[!side];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t mset = game.pieces[ps(side, 0x2)];
   while (mset) {
      uint64_t index = bsf(mset);
      mset = mset ^ PMASK[index];

      __uint128_t moveset = 0;
      __uint128_t lset = PMASK[index] & _1e(game.pieces[empty]);
      moveset = moveset | _2w1n(lset) | _2w1s(lset);
      __uint128_t rset = PMASK[index] & _1w(game.pieces[empty]);
      moveset = moveset | _2e1n(rset) | _2e1s(rset);
      __uint128_t flset = PMASK[index] & _1s(game.pieces[empty]);
      moveset = moveset | _2n1w(flset);
      __uint128_t frset = PMASK[index] & _1s(game.pieces[empty]);
      moveset = moveset | _2n1e(frset);
      __uint128_t blset = PMASK[index] & _1n(game.pieces[empty]);
      moveset = moveset | _2s1w(blset);
      __uint128_t brset = PMASK[index] & _1n(game.pieces[empty]);
      moveset = moveset | _2s1e(brset);
      moveset = moveset & BMASK & game.occupancy[!side];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t pset = game.pieces[ps(side, 0x3)];
   while (pset) {
      uint64_t index = bsf(pset);
      pset = pset ^ PMASK[index];

      __uint128_t rset, rlow, rcbn;
      __uint128_t fset, flow, fcbn;
      __uint128_t moveset;

      __uint128_t occupancy = (~game.pieces[empty] ^ PMASK[index]);
      __uint128_t outer = occupancy | OMASK[index];

      rlow = outer & LMASK[index];
      rcbn = C3U128 << bsr(rlow);
      rset = ~(outer ^ (outer - rcbn)) & occupancy;

      fset = outer & FMASK[index];
      flow = rlow & FMASK[index];
      fcbn = C3U128 << bsr(flow);
      fset = ~(fset ^ (fset - fcbn)) & occupancy;
      fset = fset & FMASK[index];
      flow = fset & LMASK[index];
      fcbn = C3U128 << bsr(flow);
      fset = fset & ~(fset - fcbn);

      rlow = rset & LMASK[index];
      rcbn = C3U128 << bsr(rlow);
      rset = rset & ~(rset - rcbn);

      moveset = (rset & RMASK[index]) | (fset & FMASK[index]);
      moveset = moveset & game.occupancy[!side];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t zset;
   zset = _1f(game.pieces[ps(side, 0x6)], side);
   zset &= ZMASK[side] & game.occupancy[!side];
   add_shiftwise(zset, side ? -WIDTH : WIDTH, &moves);

   zset = _1e(game.pieces[ps(side, 0x6)]) & BMASK;
   zset &= ZMASK[side] & game.occupancy[!side];
   add_shiftwise(zset, 1, &moves);

   zset = _1w(game.pieces[ps(side, 0x6)]) & BMASK;
   zset &= ZMASK[side] & game.occupancy[!side];
   add_shiftwise(zset, -1, &moves);

   __uint128_t xset = game.pieces[ps(side, 0x5)];
   while (xset) {
      uint64_t index = bsf(xset);
      xset = xset ^ PMASK[index];

      __uint128_t moveset;
      moveset = (_2n2w(PMASK[index]) & _1n1w(game.pieces[empty]))
         | (_2n2e(PMASK[index]) & _1n1e(game.pieces[empty]))
         | (_2s2e(PMASK[index]) & _1s1e(game.pieces[empty]))
         | (_2s2w(PMASK[index]) & _1s1w(game.pieces[empty]));
      moveset = moveset & XMASK[side] & game.occupancy[!side];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t sset = game.pieces[ps(side, 0x4)];
   while (sset) {
      uint64_t index = bsf(sset);
      sset = sset ^ PMASK[index];

      __uint128_t moveset;
      moveset = _1n1w(PMASK[index]) | _1n1e(PMASK[index])
         | _1s1e(PMASK[index]) | _1s1w(PMASK[index]);
      moveset = moveset & SMASK[side] & game.occupancy[!side];

      add_piecewise(moveset, index, &moves);
   }

   {
      uint64_t index = bsf(game.pieces[ps(side, 0x0)]);
      __uint128_t moveset;
      moveset = _1n(game.pieces[ps(side, 0x0)])
         | _1s(game.pieces[ps(side, 0x0)])
         | _1e(game.pieces[ps(side, 0x0)])
         | _1w(game.pieces[ps(side, 0x0)]);
      moveset = moveset & JMASK[side] & game.occupancy[!side];

      __uint128_t fly = (game.pieces[ps(black, 0x0)] << 1)
         - game.pieces[ps(red, 0x0)];
      fly = fly & FMASK[index] & ~game.pieces[empty];
      fly ^= game.pieces[ps(red, 0x0)] | game.pieces[ps(black, 0x0)];
      moveset = moveset | (fly ? 0 : game.pieces[po(side, 0x0)]);
      add_piecewise(moveset, index, &moves);
   }

   return moves;
}

move_array_t generate_quiet(int64_t side) {
   move_array_t moves = {malloc(111 * sizeof(move_t)), 0, 0};
   if (!game.pieces[ps(side, 0x0)]) { return moves; }

   __uint128_t C3U128 = 0x3;

   __uint128_t jpset = game.pieces[ps(side, 0x1)] | game.pieces[ps(side, 0x3)];
   while (jpset) {
      uint64_t index = bsf(jpset);
      jpset = jpset ^ PMASK[index];

      __uint128_t rset, rlow, rcbn;
      __uint128_t fset, flow, fcbn;
      __uint128_t moveset;

      __uint128_t outer = (~game.pieces[empty] ^ PMASK[index]) | OMASK[index];

      rlow = outer & LMASK[index];
      rcbn = C3U128 << bsr(rlow);
      rset = outer ^ (outer - rcbn);

      flow = rlow & FMASK[index];
      fcbn = C3U128 << bsr(flow);
      fset = outer & FMASK[index];
      fset = fset ^ (fset - fcbn);

      moveset = (rset & RMASK[index]) | (fset & FMASK[index]);
      moveset = moveset & game.pieces[empty];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t mset = game.pieces[ps(side, 0x2)];
   while (mset) {
      uint64_t index = bsf(mset);
      mset = mset ^ PMASK[index];

      __uint128_t moveset = 0;
      __uint128_t lset = PMASK[index] & _1e(game.pieces[empty]);
      moveset = moveset | _2w1n(lset) | _2w1s(lset);
      __uint128_t rset = PMASK[index] & _1w(game.pieces[empty]);
      moveset = moveset | _2e1n(rset) | _2e1s(rset);
      __uint128_t flset = PMASK[index] & _1s(game.pieces[empty]);
      moveset = moveset | _2n1w(flset);
      __uint128_t frset = PMASK[index] & _1s(game.pieces[empty]);
      moveset = moveset | _2n1e(frset);
      __uint128_t blset = PMASK[index] & _1n(game.pieces[empty]);
      moveset = moveset | _2s1w(blset);
      __uint128_t brset = PMASK[index] & _1n(game.pieces[empty]);
      moveset = moveset | _2s1e(brset);
      moveset = moveset & BMASK & game.pieces[empty];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t zset;
   zset = _1f(game.pieces[ps(side, 0x6)], side);
   zset &= ZMASK[side] & game.pieces[empty];
   add_shiftwise(zset, side ? -WIDTH : WIDTH, &moves);

   zset = _1e(game.pieces[ps(side, 0x6)]) & BMASK;
   zset &= ZMASK[side] & game.pieces[empty];
   add_shiftwise(zset, 1, &moves);

   zset = _1w(game.pieces[ps(side, 0x6)]) & BMASK;
   zset &= ZMASK[side] & game.pieces[empty];
   add_shiftwise(zset, -1, &moves);

   __uint128_t xset = game.pieces[ps(side, 0x5)];
   while (xset) {
      uint64_t index = bsf(xset);
      xset = xset ^ PMASK[index];

      __uint128_t moveset;
      moveset = (_2n2w(PMASK[index]) & _1n1w(game.pieces[empty]))
         | (_2n2e(PMASK[index]) & _1n1e(game.pieces[empty]))
         | (_2s2e(PMASK[index]) & _1s1e(game.pieces[empty]))
         | (_2s2w(PMASK[index]) & _1s1w(game.pieces[empty]));
      moveset = moveset & XMASK[side] & game.pieces[empty];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t sset = game.pieces[ps(side, 0x4)];
   while (sset) {
      uint64_t index = bsf(sset);
      sset = sset ^ PMASK[index];

      __uint128_t moveset;
      moveset = _1n1w(PMASK[index]) | _1n1e(PMASK[index])
         | _1s1e(PMASK[index]) | _1s1w(PMASK[index]);
      moveset = moveset & SMASK[side] & game.pieces[empty];

      add_piecewise(moveset, index, &moves);
   }

   {
      __uint128_t moveset;
      moveset = _1n(game.pieces[ps(side, 0x0)])
         | _1s(game.pieces[ps(side, 0x0)])
         | _1e(game.pieces[ps(side, 0x0)])
         | _1w(game.pieces[ps(side, 0x0)]);
      moveset = moveset & JMASK[side] & game.pieces[empty];
      add_piecewise(moveset, bsf(
         game.pieces[ps(side, 0x0)]), &moves);
   }

   return moves;
}

void add_piecewise(__uint128_t set, uint64_t from, move_array_t* moves) {
   for (; set; set &= set - 1) {
      uint64_t to = bsf(set);
      move_t move = { ._ = { from, to, board[from], board[to] } };
      moves->data[moves->count++] = move;
   }
}

void add_shiftwise(__uint128_t set, int64_t shift, move_array_t* moves) {
   for (; set; set &= set - 1) {
      uint64_t to = bsf(set);
      uint64_t from = to - shift;
      move_t move = { ._ = { from, to, board[from], board[to] } };
      moves->data[moves->count++] = move;
   }
}

void sort_moves(move_array_t* moves) {
   if (!moves->count) { return; }

   move_t sorted[111];

   int64_t indices[9] = {0};
   int64_t* counts = &indices[1];
   for (int64_t i = 0; i != moves->count; ++i)
      ++counts[p(moves->data[i]._.pto)];
   for (int64_t i = 1; i != 7; ++i)
      counts[i] = counts[i] + counts[i - 1];
   moves->quiet = counts[6];
   for (int64_t i = 0; i != moves->count; ++i)
      sorted[indices[p(moves->data[i]._.pto)]++] = moves->data[i];

   memcpy(moves->data, sorted, moves->count * sizeof(move_t));
}

move_t next(generator_t* engine, transient_t* state) {
   switch (engine->state) {
      case 0:
         ++(engine->state);
         if (engine->move.bits)
            return engine->move;
      case 1:
         ++(engine->state);
         engine->moves = generate_pseudolegal(state->side);
         sort_moves(&(engine->moves));
      case 2:
         if (engine->index < engine->moves.quiet)
            return engine->moves.data[engine->index++];
      case 3:
         ++(engine->state);
         ++(engine->state);
         move_t first = ktable[state->ply].first;
         if (first.bits && is_valid(first, state->side))
            return first;
      case 4:
         ++(engine->state);
         move_t second = ktable[state->ply].second;
         if (second.bits && is_valid(second, state->side))
            return second;
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
