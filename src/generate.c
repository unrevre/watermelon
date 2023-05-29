#include "generate.h"

#include "bucket.h"
#include "inlines.h"
#include "magics.h"
#include "masks.h"

#include <stdint.h>
#include <string.h>

/*!
 * add_piecewise
 * @ add moves (for fixed piece)
 */

static void add_piecewise(struct transient_t* state, __uint128_t set,
                          uint64_t from, struct move_array_t* moves) {
   for (; set; set &= set - 1) {
      uint64_t to = bsf(set);
      union move_t move = { ._ = {
         from, to, state->board[from], state->board[to] } };
      moves->data[moves->count++] = move;
   }
}

/*!
 * add_shiftwise
 * @ add moves (for fixed direction)
 */

static void add_shiftwise(struct transient_t* state, __uint128_t set,
                          int64_t shift, struct move_array_t* moves) {
   for (; set; set &= set - 1) {
      uint64_t to = bsf(set);
      uint64_t from = to - shift;
      union move_t move = { ._ = {
         from, to, state->board[from], state->board[to] } };
      moves->data[moves->count++] = move;
   }
}

struct move_array_t generate_pseudolegal(struct transient_t* state) {
   int64_t side = state->side;

   int64_t slot = acquire_slot(&bucket);
   union move_t* data = (union move_t*)&((struct set_t*)bucket.data)[slot];

   struct move_array_t moves = {data, slot, 0, 0};
   if (!state->pieces[ps(side, 0x0)]) { return moves; }

   __uint128_t C3U128 = 0x3;

   __uint128_t jset = state->pieces[ps(side, 0x1)];
   while (jset) {
      uint64_t index = bsf(jset);
      jset = jset ^ PMASK[index];

      __uint128_t rset, rlow, rcbn;
      __uint128_t fset, flow, fcbn;
      __uint128_t moveset;

      __uint128_t outer = (~state->pieces[empty] ^ PMASK[index]) | OMASK;

      rlow = outer & LMASK[index];
      rcbn = C3U128 << bsr(rlow);
      rset = outer ^ (outer - rcbn);

      flow = rlow & FMASK[index];
      fcbn = C3U128 << bsr(flow);
      fset = outer & FMASK[index];
      fset = fset ^ (fset - fcbn);

      moveset = (rset & RMASK[index]) | (fset & FMASK[index]);
      moveset = moveset & ~state->occupancy[side];

      add_piecewise(state, moveset, index, &moves);
   }

   __uint128_t mset = state->pieces[ps(side, 0x2)];
   while (mset) {
      uint64_t index = bsf(mset);
      mset = mset ^ PMASK[index];

      __uint128_t moveset = 0;
      __uint128_t lset = PMASK[index] & s1e(state->pieces[empty]);
      moveset = moveset | s1n2w(lset) | s1s2w(lset);
      __uint128_t rset = PMASK[index] & s1w(state->pieces[empty]);
      moveset = moveset | s1n2e(rset) | s1s2e(rset);
      __uint128_t nset = PMASK[index] & s1s(state->pieces[empty]);
      moveset = moveset | s2n1w(nset) | s2n1e(nset);
      __uint128_t sset = PMASK[index] & s1n(state->pieces[empty]);
      moveset = moveset | s2s1w(sset) | s2s1e(sset);
      moveset = moveset & BMASK & ~state->occupancy[side];

      add_piecewise(state, moveset, index, &moves);
   }

   __uint128_t pset = state->pieces[ps(side, 0x3)];
   while (pset) {
      uint64_t index = bsf(pset);
      pset = pset ^ PMASK[index];

      __uint128_t rset, rlow, rcbn;
      __uint128_t fset, flow, fcbn;
      __uint128_t moveset, captures;

      __uint128_t occupancy = (~state->pieces[empty] ^ PMASK[index]);
      __uint128_t outer = occupancy | OMASK;

      rlow = outer & LMASK[index];
      rcbn = C3U128 << bsr(rlow);
      rset = outer ^ (outer - rcbn);

      flow = rlow & FMASK[index];
      fcbn = C3U128 << bsr(flow);
      fset = outer & FMASK[index];
      fset = fset ^ (fset - fcbn);

      moveset = (rset & RMASK[index]) | (fset & FMASK[index]);
      moveset = moveset & state->pieces[empty];

      rset = ~rset & occupancy;
      rlow = rset & LMASK[index];
      rcbn = C3U128 << bsr(rlow);
      rset = rset & ~(rset - rcbn);

      fset = (~fset & occupancy) & FMASK[index];
      flow = fset & LMASK[index];
      fcbn = C3U128 << bsr(flow);
      fset = fset & ~(fset - fcbn);

      captures = (rset & RMASK[index]) | (fset & FMASK[index]);
      captures = captures & state->occupancy[!side];
      moveset = moveset | captures;

      add_piecewise(state, moveset, index, &moves);
   }

   __uint128_t zset;
   zset = s1f(state->pieces[ps(side, 0x6)], side);
   zset &= ZMASK[side] & ~state->occupancy[side];
   add_shiftwise(state, zset, d1f(side), &moves);

   zset = s1e(state->pieces[ps(side, 0x6)]) & BMASK;
   zset &= ZMASK[side] & ~state->occupancy[side];
   add_shiftwise(state, zset, 1, &moves);

   zset = s1w(state->pieces[ps(side, 0x6)]) & BMASK;
   zset &= ZMASK[side] & ~state->occupancy[side];
   add_shiftwise(state, zset, -1, &moves);

   __uint128_t xset;
   xset = s2n2e(state->pieces[ps(side, 0x5)]) & s1n1e(state->pieces[empty]);
   xset = xset & XMASK[side] & ~state->occupancy[side];
   add_shiftwise(state, xset, d2n2e, &moves);

   xset = s2n2w(state->pieces[ps(side, 0x5)]) & s1n1w(state->pieces[empty]);
   xset = xset & XMASK[side] & ~state->occupancy[side];
   add_shiftwise(state, xset, d2n2w, &moves);

   xset = s2s2e(state->pieces[ps(side, 0x5)]) & s1s1e(state->pieces[empty]);
   xset = xset & XMASK[side] & ~state->occupancy[side];
   add_shiftwise(state, xset, d2s2e, &moves);

   xset = s2s2w(state->pieces[ps(side, 0x5)]) & s1s1w(state->pieces[empty]);
   xset = xset & XMASK[side] & ~state->occupancy[side];
   add_shiftwise(state, xset, d2s2w, &moves);

   __uint128_t sset;
   sset = s1n1e(state->pieces[ps(side, 0x4)]);
   sset = sset & SMASK[side] & ~state->occupancy[side];
   add_shiftwise(state, sset, d1n1e, &moves);

   sset = s1n1w(state->pieces[ps(side, 0x4)]);
   sset = sset & SMASK[side] & ~state->occupancy[side];
   add_shiftwise(state, sset, d1n1w, &moves);

   sset = s1s1e(state->pieces[ps(side, 0x4)]);
   sset = sset & SMASK[side] & ~state->occupancy[side];
   add_shiftwise(state, sset, d1s1e, &moves);

   sset = s1s1w(state->pieces[ps(side, 0x4)]);
   sset = sset & SMASK[side] & ~state->occupancy[side];
   add_shiftwise(state, sset, d1s1w, &moves);

   {
      uint64_t index = bsf(state->pieces[ps(side, 0x0)]);

      __uint128_t moveset;
      moveset = i1n(index) | i1s(index) | i1e(index) | i1w(index);
      moveset = moveset & JMASK[side] & ~state->occupancy[side];

      __uint128_t fly = ((state->pieces[ps(black, 0x0)] << 1)
         - state->pieces[ps(red, 0x0)]) & FMASK[index] & ~state->pieces[empty];
      fly = fly ^ state->pieces[ps(red, 0x0)] ^ state->pieces[ps(black, 0x0)];
      moveset = moveset | (fly ? 0 : state->pieces[po(side, 0x0)]);

      add_piecewise(state, moveset, index, &moves);
   }

   return moves;
}

struct move_array_t generate_captures(struct transient_t* state) {
   int64_t side = state->side;

   int64_t slot = acquire_slot(&bucket);
   union move_t* data = (union move_t*)&((struct set_t*)bucket.data)[slot];

   struct move_array_t moves = {data, slot, 0, 0};
   if (!state->pieces[ps(side, 0x0)]) { return moves; }

   __uint128_t C3U128 = 0x3;

   __uint128_t jset = state->pieces[ps(side, 0x1)];
   while (jset) {
      uint64_t index = bsf(jset);
      jset = jset ^ PMASK[index];

      __uint128_t rset, rlow, rcbn;
      __uint128_t fset, flow, fcbn;
      __uint128_t moveset;

      __uint128_t outer = (~state->pieces[empty] ^ PMASK[index]) | OMASK;

      rlow = outer & LMASK[index];
      rcbn = C3U128 << bsr(rlow);
      rset = outer ^ (outer - rcbn);

      flow = rlow & FMASK[index];
      fcbn = C3U128 << bsr(flow);
      fset = outer & FMASK[index];
      fset = fset ^ (fset - fcbn);

      moveset = (rset & RMASK[index]) | (fset & FMASK[index]);
      moveset = moveset & state->occupancy[!side];

      add_piecewise(state, moveset, index, &moves);
   }

   __uint128_t mset = state->pieces[ps(side, 0x2)];
   while (mset) {
      uint64_t index = bsf(mset);
      mset = mset ^ PMASK[index];

      __uint128_t moveset = 0;
      __uint128_t lset = PMASK[index] & s1e(state->pieces[empty]);
      moveset = moveset | s1n2w(lset) | s1s2w(lset);
      __uint128_t rset = PMASK[index] & s1w(state->pieces[empty]);
      moveset = moveset | s1n2e(rset) | s1s2e(rset);
      __uint128_t nset = PMASK[index] & s1s(state->pieces[empty]);
      moveset = moveset | s2n1w(nset) | s2n1e(nset);
      __uint128_t sset = PMASK[index] & s1n(state->pieces[empty]);
      moveset = moveset | s2s1w(sset) | s2s1e(sset);
      moveset = moveset & BMASK & state->occupancy[!side];

      add_piecewise(state, moveset, index, &moves);
   }

   __uint128_t pset = state->pieces[ps(side, 0x3)];
   while (pset) {
      uint64_t index = bsf(pset);
      pset = pset ^ PMASK[index];

      __uint128_t rset, rlow, rcbn;
      __uint128_t fset, flow, fcbn;
      __uint128_t moveset;

      __uint128_t occupancy = (~state->pieces[empty] ^ PMASK[index]);
      __uint128_t outer = occupancy | OMASK;

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
      moveset = moveset & state->occupancy[!side];

      add_piecewise(state, moveset, index, &moves);
   }

   __uint128_t zset;
   zset = s1f(state->pieces[ps(side, 0x6)], side);
   zset &= ZMASK[side] & state->occupancy[!side];
   add_shiftwise(state, zset, d1f(side), &moves);

   zset = s1e(state->pieces[ps(side, 0x6)]) & BMASK;
   zset &= ZMASK[side] & state->occupancy[!side];
   add_shiftwise(state, zset, 1, &moves);

   zset = s1w(state->pieces[ps(side, 0x6)]) & BMASK;
   zset &= ZMASK[side] & state->occupancy[!side];
   add_shiftwise(state, zset, -1, &moves);

   __uint128_t xset;
   xset = s2n2e(state->pieces[ps(side, 0x5)]) & s1n1e(state->pieces[empty]);
   xset = xset & XMASK[side] & state->occupancy[!side];
   add_shiftwise(state, xset, d2n2e, &moves);

   xset = s2n2w(state->pieces[ps(side, 0x5)]) & s1n1w(state->pieces[empty]);
   xset = xset & XMASK[side] & state->occupancy[!side];
   add_shiftwise(state, xset, d2n2w, &moves);

   xset = s2s2e(state->pieces[ps(side, 0x5)]) & s1s1e(state->pieces[empty]);
   xset = xset & XMASK[side] & state->occupancy[!side];
   add_shiftwise(state, xset, d2s2e, &moves);

   xset = s2s2w(state->pieces[ps(side, 0x5)]) & s1s1w(state->pieces[empty]);
   xset = xset & XMASK[side] & state->occupancy[!side];
   add_shiftwise(state, xset, d2s2w, &moves);

   __uint128_t sset;
   sset = s1n1e(state->pieces[ps(side, 0x4)]);
   sset = sset & SMASK[side] & state->occupancy[!side];
   add_shiftwise(state, sset, d1n1e, &moves);

   sset = s1n1w(state->pieces[ps(side, 0x4)]);
   sset = sset & SMASK[side] & state->occupancy[!side];
   add_shiftwise(state, sset, d1n1w, &moves);

   sset = s1s1e(state->pieces[ps(side, 0x4)]);
   sset = sset & SMASK[side] & state->occupancy[!side];
   add_shiftwise(state, sset, d1s1e, &moves);

   sset = s1s1w(state->pieces[ps(side, 0x4)]);
   sset = sset & SMASK[side] & state->occupancy[!side];
   add_shiftwise(state, sset, d1s1w, &moves);

   {
      uint64_t index = bsf(state->pieces[ps(side, 0x0)]);

      __uint128_t moveset;
      moveset = i1n(index) | i1s(index) | i1e(index) | i1w(index);
      moveset = moveset & JMASK[side] & state->occupancy[!side];

      __uint128_t fly = ((state->pieces[ps(black, 0x0)] << 1)
         - state->pieces[ps(red, 0x0)]) & FMASK[index] & ~state->pieces[empty];
      fly = fly ^ state->pieces[ps(red, 0x0)] ^ state->pieces[ps(black, 0x0)];
      moveset = moveset | (fly ? 0 : state->pieces[po(side, 0x0)]);

      add_piecewise(state, moveset, index, &moves);
   }

   return moves;
}

void sort_moves(struct move_array_t* moves) {
   if (!moves->count) { return; }

   union move_t sorted[111];

   int64_t indices[9] = {0};
   int64_t* counts = &indices[1];
   for (int64_t i = 0; i != moves->count; ++i)
      ++counts[p(moves->data[i]._.pto)];
   for (int64_t i = 1; i != 7; ++i)
      counts[i] = counts[i] + counts[i - 1];
   moves->quiet = counts[6];
   for (int64_t i = 0; i != moves->count; ++i)
      sorted[indices[p(moves->data[i]._.pto)]++] = moves->data[i];

   memcpy(moves->data, sorted, moves->count * sizeof(union move_t));
}
