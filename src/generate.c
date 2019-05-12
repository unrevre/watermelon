#include "generate.h"

#include "inlines.h"
#include "magics.h"
#include "masks.h"
#include "memory.h"
#include "position.h"
#include "state.h"

#include <stdlib.h>
#include <string.h>

move_array_t generate_pseudolegal(transient_t* state, int64_t side) {
   move_array_t moves = {malloc(111 * sizeof(move_t)), 0, 0};
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
      __uint128_t lset = PMASK[index] & _1e(state->pieces[empty]);
      moveset = moveset | _2w1n(lset) | _2w1s(lset);
      __uint128_t rset = PMASK[index] & _1w(state->pieces[empty]);
      moveset = moveset | _2e1n(rset) | _2e1s(rset);
      __uint128_t nset = PMASK[index] & _1s(state->pieces[empty]);
      moveset = moveset | _2n1w(nset) | _2n1e(nset);
      __uint128_t sset = PMASK[index] & _1n(state->pieces[empty]);
      moveset = moveset | _2s1w(sset) | _2s1e(sset);
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
   zset = _1f(state->pieces[ps(side, 0x6)], side);
   zset &= ZMASK[side] & ~state->occupancy[side];
   add_shiftwise(state, zset, side ? -WIDTH : WIDTH, &moves);

   zset = _1e(state->pieces[ps(side, 0x6)]) & BMASK;
   zset &= ZMASK[side] & ~state->occupancy[side];
   add_shiftwise(state, zset, 1, &moves);

   zset = _1w(state->pieces[ps(side, 0x6)]) & BMASK;
   zset &= ZMASK[side] & ~state->occupancy[side];
   add_shiftwise(state, zset, -1, &moves);

   __uint128_t xset = state->pieces[ps(side, 0x5)];
   while (xset) {
      uint64_t index = bsf(xset);
      xset = xset ^ PMASK[index];

      __uint128_t moveset;
      moveset = (i2n2e(index) & _1n1e(state->pieces[empty]))
         | (i2n2w(index) & _1n1w(state->pieces[empty]))
         | (_2s2e(PMASK[index]) & _1s1e(state->pieces[empty]))
         | (_2s2w(PMASK[index]) & _1s1w(state->pieces[empty]));
      moveset = moveset & XMASK[side] & ~state->occupancy[side];

      add_piecewise(state, moveset, index, &moves);
   }

   __uint128_t sset = state->pieces[ps(side, 0x4)];
   while (sset) {
      uint64_t index = bsf(sset);
      sset = sset ^ PMASK[index];

      __uint128_t moveset;
      moveset = i1n1e(index) | i1n1w(index) | i1s1e(index) | i1s1w(index);
      moveset = moveset & SMASK[side] & ~state->occupancy[side];

      add_piecewise(state, moveset, index, &moves);
   }

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

move_array_t generate_captures(transient_t* state, int64_t side) {
   move_array_t moves = {malloc(40 * sizeof(move_t)), 0, 0};
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
      __uint128_t lset = PMASK[index] & _1e(state->pieces[empty]);
      moveset = moveset | _2w1n(lset) | _2w1s(lset);
      __uint128_t rset = PMASK[index] & _1w(state->pieces[empty]);
      moveset = moveset | _2e1n(rset) | _2e1s(rset);
      __uint128_t nset = PMASK[index] & _1s(state->pieces[empty]);
      moveset = moveset | _2n1w(nset) | _2n1e(nset);
      __uint128_t sset = PMASK[index] & _1n(state->pieces[empty]);
      moveset = moveset | _2s1w(sset) | _2s1e(sset);
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
   zset = _1f(state->pieces[ps(side, 0x6)], side);
   zset &= ZMASK[side] & state->occupancy[!side];
   add_shiftwise(state, zset, side ? -WIDTH : WIDTH, &moves);

   zset = _1e(state->pieces[ps(side, 0x6)]) & BMASK;
   zset &= ZMASK[side] & state->occupancy[!side];
   add_shiftwise(state, zset, 1, &moves);

   zset = _1w(state->pieces[ps(side, 0x6)]) & BMASK;
   zset &= ZMASK[side] & state->occupancy[!side];
   add_shiftwise(state, zset, -1, &moves);

   __uint128_t xset = state->pieces[ps(side, 0x5)];
   while (xset) {
      uint64_t index = bsf(xset);
      xset = xset ^ PMASK[index];

      __uint128_t moveset;
      moveset = (i2n2e(index) & _1n1e(state->pieces[empty]))
         | (i2n2w(index) & _1n1w(state->pieces[empty]))
         | (_2s2e(PMASK[index]) & _1s1e(state->pieces[empty]))
         | (_2s2w(PMASK[index]) & _1s1w(state->pieces[empty]));
      moveset = moveset & XMASK[side] & state->occupancy[!side];

      add_piecewise(state, moveset, index, &moves);
   }

   __uint128_t sset = state->pieces[ps(side, 0x4)];
   while (sset) {
      uint64_t index = bsf(sset);
      sset = sset ^ PMASK[index];

      __uint128_t moveset;
      moveset = i1n1e(index) | i1n1w(index) | i1s1e(index) | i1s1w(index);
      moveset = moveset & SMASK[side] & state->occupancy[!side];

      add_piecewise(state, moveset, index, &moves);
   }

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

void add_piecewise(transient_t* state, __uint128_t set, uint64_t from,
                   move_array_t* moves) {
   for (; set; set &= set - 1) {
      uint64_t to = bsf(set);
      move_t move = { ._ = {
         from, to, state->board[from], state->board[to] } };
      moves->data[moves->count++] = move;
   }
}

void add_shiftwise(transient_t* state, __uint128_t set, int64_t shift,
                   move_array_t* moves) {
   for (; set; set &= set - 1) {
      uint64_t to = bsf(set);
      uint64_t from = to - shift;
      move_t move = { ._ = {
         from, to, state->board[from], state->board[to] } };
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
         engine->moves = generate_pseudolegal(state, state->side);
         sort_moves(&(engine->moves));
      case 2:
         if (engine->index < engine->moves.quiet)
            return engine->moves.data[engine->index++];
      case 3:
         ++(engine->state);
         ++(engine->state);
         move_t first = ktable[state->ply].first;
         if (first.bits && is_valid(state, first))
            return first;
      case 4:
         ++(engine->state);
         move_t second = ktable[state->ply].second;
         if (second.bits && is_valid(state, second))
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
