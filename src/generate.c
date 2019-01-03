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
   for (; jset; jset &= jset - 1) {
      uint32_t index = bsf_branchless(jset);

      __uint128_t rset, rlow, rcbn;
      __uint128_t fset, flow, fcbn;
      __uint128_t moveset;

      __uint128_t outer = (~GAME.empty ^ PMASK[index]) | OMASK[index];

      rlow = outer & LMASK[index];
      rcbn = C3U128 << bsr_branchless(rlow);
      rset = outer ^ (outer - rcbn);

      flow = rlow & FMASK[index];
      fcbn = C3U128 << bsr_branchless(flow);
      fset = outer & FMASK[index];
      fset = fset ^ (fset - fcbn);

      moveset = (rset & RMASK[index]) | (fset & FMASK[index]);
      moveset &= ~GAME.occupancy[s];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t mset = GAME.pieces[side + 1];
   for (; mset; mset &= mset - 1) {
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
   }

   __uint128_t pset = GAME.pieces[side + 2];
   for (; pset; pset &= pset - 1) {
      uint32_t index = bsf_branchless(pset);

      __uint128_t rset, rlow, rcbn;
      __uint128_t fset, flow, fcbn;
      __uint128_t moveset;

      __uint128_t occupancy = (~GAME.empty ^ PMASK[index]);
      __uint128_t outer = occupancy | OMASK[index];

      rlow = outer & LMASK[index];
      rcbn = C3U128 << bsr_branchless(rlow);
      rset = outer ^ (outer - rcbn);

      flow = rlow & FMASK[index];
      fcbn = C3U128 << bsr_branchless(flow);
      fset = outer & FMASK[index];
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
         & GAME.occupancy[!side];

      add_piecewise(moveset, index, &moves);
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
   for (; xset; xset &= xset - 1) {
      uint32_t index = bsf_branchless(xset);

      __uint128_t moveset;
      moveset = (PMASK[index] << 0x10 & GAME.empty << 0x8)
         | (PMASK[index] << 0x14 & GAME.empty << 0xa)
         | (PMASK[index] >> 0x10 & GAME.empty >> 0x8)
         | (PMASK[index] >> 0x14 & GAME.empty >> 0xa);
      moveset &= XMASK[s] & ~GAME.occupancy[s];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t sset = GAME.pieces[side + 5];
   for (; sset; sset &= sset - 1) {
      uint32_t index = bsf_branchless(sset);

      __uint128_t moveset;
      moveset = PMASK[index] << 0x8 | PMASK[index] << 0xa
         | PMASK[index] >> 0x8 | PMASK[index] >> 0xa;
      moveset &= SMASK[s] & ~GAME.occupancy[s];

      add_piecewise(moveset, index, &moves);
   }

   {
      __uint128_t moveset;
      moveset = GAME.pieces[side + 6] << 9 | GAME.pieces[side + 6] >> 9
         | GAME.pieces[side + 6] << 1 | GAME.pieces[side + 6] >> 1;
      moveset &= JMASK[s] & ~GAME.occupancy[s];
      add_piecewise(moveset, bsf_branchless(GAME.pieces[side + 6]), &moves);
   }

   return moves;
}

move_array_t generate_pseudolegal(uint32_t side) {
   move_array_t moves = {(move_t*)malloc(111 * sizeof(move_t)), 0};
   if (!GAME.pieces[side + 6]) { return moves; }

   uint32_t s = side >> 3;

   __uint128_t C3U128 = 0x3;

   __uint128_t jset = GAME.pieces[side];
   for (; jset; jset &= jset - 1) {
      uint32_t index = bsf_branchless(jset);

      __uint128_t rset, rlow, rcbn;
      __uint128_t fset, flow, fcbn;
      __uint128_t moveset;

      __uint128_t outer = (~GAME.empty ^ PMASK[index]) | OMASK[index];

      rlow = outer & LMASK[index];
      rcbn = C3U128 << bsr_branchless(rlow);
      rset = outer ^ (outer - rcbn);

      flow = rlow & FMASK[index];
      fcbn = C3U128 << bsr_branchless(flow);
      fset = outer & FMASK[index];
      fset = fset ^ (fset - fcbn);

      moveset = (rset & RMASK[index]) | (fset & FMASK[index]);
      moveset &= ~GAME.occupancy[s];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t mset = GAME.pieces[side + 1];
   for (; mset; mset &= mset - 1) {
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
   }

   __uint128_t pset = GAME.pieces[side + 2];
   for (; pset; pset &= pset - 1) {
      uint32_t index = bsf_branchless(pset);

      __uint128_t rset, rlow, rcbn;
      __uint128_t fset, flow, fcbn;
      __uint128_t moveset;

      __uint128_t occupancy = (~GAME.empty ^ PMASK[index]);
      __uint128_t outer = occupancy | OMASK[index];

      rlow = outer & LMASK[index];
      rcbn = C3U128 << bsr_branchless(rlow);
      rset = outer ^ (outer - rcbn);

      flow = rlow & FMASK[index];
      fcbn = C3U128 << bsr_branchless(flow);
      fset = outer & FMASK[index];
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
         & GAME.occupancy[!side];

      add_piecewise(moveset, index, &moves);
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
   for (; xset; xset &= xset - 1) {
      uint32_t index = bsf_branchless(xset);

      __uint128_t moveset;
      moveset = (PMASK[index] << 0x10 & GAME.empty << 0x8)
         | (PMASK[index] << 0x14 & GAME.empty << 0xa)
         | (PMASK[index] >> 0x10 & GAME.empty >> 0x8)
         | (PMASK[index] >> 0x14 & GAME.empty >> 0xa);
      moveset &= XMASK[s] & ~GAME.occupancy[s];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t sset = GAME.pieces[side + 5];
   for (; sset; sset &= sset - 1) {
      uint32_t index = bsf_branchless(sset);

      __uint128_t moveset;
      moveset = PMASK[index] << 0x8 | PMASK[index] << 0xa
         | PMASK[index] >> 0x8 | PMASK[index] >> 0xa;
      moveset &= SMASK[s] & ~GAME.occupancy[s];

      add_piecewise(moveset, index, &moves);
   }

   {
      uint32_t index = bsf_branchless(GAME.pieces[side + 6]);
      __uint128_t moveset;
      moveset = GAME.pieces[side + 6] << 9 | GAME.pieces[side + 6] >> 9
         | GAME.pieces[side + 6] << 1 | GAME.pieces[side + 6] >> 1;
      moveset &= JMASK[s] & ~GAME.occupancy[s];
      add_piecewise(moveset, index, &moves);

      __uint128_t fly = (GAME.pieces[0xe] << 1) - GAME.pieces[0x6] + 0x1;
      fly &= FMASK[index] & ~GAME.empty;
      fly ^= GAME.pieces[0x6] | GAME.pieces[0xe];
      if (!fly) { add_piecewise(GAME.pieces[side ^ 0xe], index, &moves); }
   }

   return moves;
}

move_array_t generate_captures(uint32_t side) {
   move_array_t moves = {(move_t*)malloc(53 * sizeof(move_t)), 0};
   if (!GAME.pieces[side + 6]) { return moves; }

   __uint128_t C3U128 = 0x3;

   __uint128_t jset = GAME.pieces[side];
   for (; jset; jset &= jset - 1) {
      uint32_t index = bsf_branchless(jset);

      __uint128_t rset, rlow, rcbn;
      __uint128_t fset, flow, fcbn;
      __uint128_t moveset;

      __uint128_t outer = (~GAME.empty ^ PMASK[index]) | OMASK[index];

      rlow = outer & LMASK[index];
      rcbn = C3U128 << bsr_branchless(rlow);
      rset = outer ^ (outer - rcbn);

      flow = rlow & FMASK[index];
      fcbn = C3U128 << bsr_branchless(flow);
      fset = outer & FMASK[index];
      fset = fset ^ (fset - fcbn);

      moveset = (rset & RMASK[index]) | (fset & FMASK[index]);
      moveset &= GAME.occupancy[!side];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t mset = GAME.pieces[side + 1];
   for (; mset; mset &= mset - 1) {
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
      moveset &= BMASK & GAME.occupancy[!side];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t pset = GAME.pieces[side + 2];
   for (; pset; pset &= pset - 1) {
      uint32_t index = bsf_branchless(pset);

      __uint128_t rset, rlow, rcbn;
      __uint128_t fset, flow, fcbn;
      __uint128_t moveset;

      __uint128_t occupancy = (~GAME.empty ^ PMASK[index]);
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
         & GAME.occupancy[!side];

      add_piecewise(moveset, index, &moves);
   }

   uint32_t s = side >> 3;

   __uint128_t zset;
   zset = (GAME.pieces[side + 3] << 9) >> (18 * s);
   zset &= ZMASK[s] & GAME.occupancy[!side];
   add_shiftwise(zset, 9 - 18 * s, &moves);

   zset = GAME.pieces[side + 3] << 1 & FMASKN0;
   zset &= ZMASK[s] & GAME.occupancy[!side];
   add_shiftwise(zset, 1, &moves);

   zset = GAME.pieces[side + 3] >> 1 & FMASKN8;
   zset &= ZMASK[s] & GAME.occupancy[!side];
   add_shiftwise(zset, -1, &moves);

   __uint128_t xset = GAME.pieces[side + 4];
   for (; xset; xset &= xset - 1) {
      uint32_t index = bsf_branchless(xset);

      __uint128_t moveset;
      moveset = (PMASK[index] << 0x10 & GAME.empty << 0x8)
         | (PMASK[index] << 0x14 & GAME.empty << 0xa)
         | (PMASK[index] >> 0x10 & GAME.empty >> 0x8)
         | (PMASK[index] >> 0x14 & GAME.empty >> 0xa);
      moveset &= XMASK[s] & GAME.occupancy[!side];

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t sset = GAME.pieces[side + 5];
   for (; sset; sset &= sset - 1) {
      uint32_t index = bsf_branchless(sset);

      __uint128_t moveset;
      moveset = PMASK[index] << 0x8 | PMASK[index] << 0xa
         | PMASK[index] >> 0x8 | PMASK[index] >> 0xa;
      moveset &= SMASK[s] & GAME.occupancy[!side];

      add_piecewise(moveset, index, &moves);
   }

   {
      uint32_t index = bsf_branchless(GAME.pieces[side + 6]);
      __uint128_t moveset;
      moveset = GAME.pieces[side + 6] << 9 | GAME.pieces[side + 6] >> 9
         | GAME.pieces[side + 6] << 1 | GAME.pieces[side + 6] >> 1;
      moveset &= JMASK[s] & GAME.occupancy[!side];
      add_piecewise(moveset, index, &moves);

      __uint128_t fly = (GAME.pieces[0xe] << 1) - GAME.pieces[0x6] + 0x1;
      fly &= FMASK[index] & ~GAME.empty;
      fly ^= GAME.pieces[0x6] | GAME.pieces[0xe];
      if (!fly) { add_piecewise(GAME.pieces[side ^ 0xe], index, &moves); }
   }

   return moves;
}

move_array_t generate_quiet(uint32_t side) {
   move_array_t moves = {(move_t*)malloc(111 * sizeof(move_t)), 0};

   uint32_t s = side >> 3;

   __uint128_t C3U128 = 0x3;

   __uint128_t jpset = GAME.pieces[side] | GAME.pieces[side + 2];
   for (; jpset; jpset &= jpset - 1) {
      uint32_t index = bsf_branchless(jpset);

      __uint128_t rset, rlow, rcbn;
      __uint128_t fset, flow, fcbn;
      __uint128_t moveset;

      __uint128_t outer = (~GAME.empty ^ PMASK[index]) | OMASK[index];

      rlow = outer & LMASK[index];
      rcbn = C3U128 << bsr_branchless(rlow);
      rset = outer ^ (outer - rcbn);

      flow = rlow & FMASK[index];
      fcbn = C3U128 << bsr_branchless(flow);
      fset = outer & FMASK[index];
      fset = fset ^ (fset - fcbn);

      moveset = (rset & RMASK[index]) | (fset & FMASK[index]);
      moveset &= GAME.empty;

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t mset = GAME.pieces[side + 1];
   for (; mset; mset &= mset - 1) {
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
      moveset &= BMASK & GAME.empty;

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t zset;
   zset = (GAME.pieces[side + 3] << 9) >> (18 * s);
   zset &= ZMASK[s] & GAME.empty;
   add_shiftwise(zset, 9 - 18 * s, &moves);

   zset = GAME.pieces[side + 3] << 1 & FMASKN0;
   zset &= ZMASK[s] & GAME.empty;
   add_shiftwise(zset, 1, &moves);

   zset = GAME.pieces[side + 3] >> 1 & FMASKN8;
   zset &= ZMASK[s] & GAME.empty;
   add_shiftwise(zset, -1, &moves);

   __uint128_t xset = GAME.pieces[side + 4];
   for (; xset; xset &= xset - 1) {
      uint32_t index = bsf_branchless(xset);

      __uint128_t moveset;
      moveset = (PMASK[index] << 0x10 & GAME.empty << 0x8)
         | (PMASK[index] << 0x14 & GAME.empty << 0xa)
         | (PMASK[index] >> 0x10 & GAME.empty >> 0x8)
         | (PMASK[index] >> 0x14 & GAME.empty >> 0xa);
      moveset &= XMASK[s] & GAME.empty;

      add_piecewise(moveset, index, &moves);
   }

   __uint128_t sset = GAME.pieces[side + 5];
   for (; sset; sset &= sset - 1) {
      uint32_t index = bsf_branchless(sset);

      __uint128_t moveset;
      moveset = PMASK[index] << 0x8 | PMASK[index] << 0xa
         | PMASK[index] >> 0x8 | PMASK[index] >> 0xa;
      moveset &= SMASK[s] & GAME.empty;

      add_piecewise(moveset, index, &moves);
   }

   {
      __uint128_t moveset;
      moveset = GAME.pieces[side + 6] << 9 | GAME.pieces[side + 6] >> 9
         | GAME.pieces[side + 6] << 1 | GAME.pieces[side + 6] >> 1;
      moveset &= JMASK[s] & GAME.empty;
      add_piecewise(moveset, bsf_branchless(GAME.pieces[side + 6]), &moves);
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
   uint32_t index = bsf_branchless(GAME.pieces[side + 6]);

   __uint128_t jset = GAME.pieces[side ^ 0x8] | GAME.pieces[side ^ 0xe];
   __uint128_t jrset = jset & RMASK[index];
   __uint128_t jfset = jset & FMASK[index];

   __uint128_t xmask = PMASK[index] | GAME.empty;
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

   __uint128_t pset = GAME.pieces[side ^ 0xa];
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

   __uint128_t mset = (PMASK[index] << 0x11 & GAME.empty << 0x9)
      | (PMASK[index] >> 0x11 & GAME.empty >> 0x9)
      | (PMASK[index] << 0x13 & GAME.empty << 0x9)
      | (PMASK[index] >> 0x13 & GAME.empty >> 0x9)
      | (PMASK[index] << 0x07 & GAME.empty >> 0x1)
      | (PMASK[index] >> 0x07 & GAME.empty << 0x1)
      | (PMASK[index] << 0x0b & GAME.empty << 0x1)
      | (PMASK[index] >> 0x0b & GAME.empty >> 0x1);
   mset = mset & GAME.pieces[side ^ 0x9];
   if (mset) { return 1; }

   __uint128_t zset = (PMASK[index] << 9) >> (18 * side >> 3)
      | PMASK[index] << 1 | PMASK[index] >> 1;
   zset = zset & GAME.pieces[side ^ 0xb];
   if (zset) { return 1; }

   return 0;
}

uint32_t is_legal(move_t move, uint32_t side) {
   if (side != (move._.pfrom & 0x8)) { return 0; }

   uint32_t from = move._.from;
   if (board[from] != move._.pfrom) { return 0; }

   uint32_t to = move._.to;
   if (board[to] != move._.pto) { return 0; }

   switch (move._.pfrom & 0x7) {
      case 0: {
         uint32_t high = max(from, to);
         uint32_t low = min(from, to);

         __uint128_t jspan = PMASK[high] - (PMASK[low] << 1);
         if (high - low > 8) { jspan &= FMASK[high]; }
         jspan &= ~GAME.empty;
         return !jspan; }
      case 1:
         switch (from - to) {
            case -19: case -11: { return board[to - 10] == 0x7; }
            case -17: case -7: { return board[to - 8] == 0x7; }
            case 7: case 17: { return board[to + 8] == 0x7; }
            case 11: case 19: { return board[to + 10] == 0x7; }
            default: { return 0; }
         }
      case 2: {
         uint32_t high = max(from, to);
         uint32_t low = min(from, to);

         __uint128_t pspan = PMASK[high] - (PMASK[low] << 1);
         if (high - low > 8) { pspan &= FMASK[high]; }
         pspan &= ~GAME.empty;

         if (move._.pto == 0x7) { return !pspan; }
         else { return popcnt(pspan) == 1; } }
      case 3: return 1;
      case 4: return board[(from + to) / 2] == 0x7;
      case 5: return 1;
      case 6: {
         if ((move._.pto & 0x7) != 6) { return 1; }
         __uint128_t jspan = GAME.pieces[0xe] - (GAME.pieces[0x8] << 1);
         jspan &= FMASK[from] & ~GAME.empty;
         return !jspan; }
      default: return 0;
   }
}

void advance(move_t move) {
   hash_state ^= hashes[move._.pfrom][move._.from];
   hash_state ^= hashes[move._.pfrom][move._.to];
   hash_state ^= hashes[move._.pto][move._.to];
   hash_state ^= hash_move;

   uint32_t s = move._.pfrom >> 3;
   GAME.pieces[move._.pfrom] ^= PMASK[move._.from] | PMASK[move._.to];
   GAME.pieces[move._.pto] ^= PMASK[move._.to];
   GAME.pieces[0x7] = 0x0;

   GAME.occupancy[s] ^= PMASK[move._.from] | PMASK[move._.to];
   GAME.occupancy[!s] ^= GAME.occupancy[0] & GAME.occupancy[1];
   GAME.empty = ~(GAME.occupancy[0] | GAME.occupancy[1]);

   board[move._.from] = 0x7;
   board[move._.to] = move._.pfrom;
}

void retract(move_t move) {
   hash_state ^= hashes[move._.pfrom][move._.from];
   hash_state ^= hashes[move._.pfrom][move._.to];
   hash_state ^= hashes[move._.pto][move._.to];
   hash_state ^= hash_move;

   uint32_t s = move._.pfrom >> 3;
   GAME.pieces[move._.pfrom] ^= PMASK[move._.from] | PMASK[move._.to];
   GAME.pieces[move._.pto] ^= PMASK[move._.to];
   GAME.pieces[0x7] = 0x0;

   GAME.occupancy[s] ^= PMASK[move._.from] | PMASK[move._.to];
   GAME.occupancy[!s] |= GAME.pieces[move._.pto];
   GAME.empty = ~(GAME.occupancy[0] | GAME.occupancy[1]);

   board[move._.from] = move._.pfrom;
   board[move._.to] = move._.pto;
}
