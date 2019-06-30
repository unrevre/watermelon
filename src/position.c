#include "position.h"

#include "inlines.h"
#include "magics.h"
#include "masks.h"
#include "state.h"
#include "structs.h"

#include <stdlib.h>

uint32_t in_check(struct transient_t* state, int64_t side) {
   uint64_t index = bsf(state->pieces[ps(side, 0x0)]);

   __uint128_t pmask = PMASK[index];
   __uint128_t umask = UMASK[index];
   __uint128_t rmask = RMASK[index];
   __uint128_t fmask = FMASK[index];
   __uint128_t emask = state->pieces[empty];
   __uint128_t xmask = pmask | emask;

   __uint128_t pset = state->pieces[po(side, 0x3)];

   __uint128_t prset = pset & rmask;
   for (; prset; prset &= prset - 1) {
      __uint128_t range = prset ^ -prset ^ umask;
      __uint128_t lsb = prset & -prset;
      __uint128_t pxmask = ~(lsb | xmask);
      range = range & pxmask & rmask;
      if (popcnt(range) == 1) { return 1; }
   }

   __uint128_t pfset = pset & fmask;
   for (; pfset; pfset &= pfset - 1) {
      __uint128_t range = pfset ^ -pfset ^ umask;
      __uint128_t lsb = pfset & -pfset;
      __uint128_t pxmask = ~(lsb | xmask);
      range = range & pxmask & fmask;
      if (popcnt(range) == 1) { return 1; }
   }

   __uint128_t jset = state->pieces[po(side, 0x0)]
      | state->pieces[po(side, 0x1)];
   __uint128_t jxmask = ~(jset | xmask);

   __uint128_t jrset = jset & rmask;
   for (; jrset; jrset &= jrset - 1) {
      __uint128_t range = jrset ^ -jrset ^ umask;
      range = range & jxmask & rmask;
      if (!range) { return 1; }
   }

   __uint128_t jfset = jset & fmask;
   for (; jfset; jfset &= jfset - 1) {
      __uint128_t range = jfset ^ -jfset ^ umask;
      range = range & jxmask & fmask;
      if (!range) { return 1; }
   }

   __uint128_t mset = ((i2n1w(index) | i2n1e(index)) & s1n(emask))
      | ((s2s1e(pmask) | s2s1w(pmask)) & s1s(emask))
      | ((i1s2e(index) | i1n2e(index)) & s1e(emask))
      | ((i1n2w(index) | s1s2w(pmask)) & s1w(emask));
   mset = mset & state->pieces[po(side, 0x2)];
   if (mset) { return 1; }

   __uint128_t zset = s1f(pmask, side) | i1e(index) | i1w(index);
   zset = zset & state->pieces[po(side, 0x6)];
   if (zset) { return 1; }

   return 0;
}

uint32_t is_valid(struct transient_t* state, union move_t move) {
   int64_t side = state->side;
   if (!state->pieces[ps(side, 0x0)]) { return 0; }
   if (side != s(move._.pfrom)) { return 0; }

   int32_t from = move._.from;
   if (state->board[from] != move._.pfrom) { return 0; }

   int32_t to = move._.to;
   if (state->board[to] != move._.pto) { return 0; }

   switch (p(move._.pfrom)) {
      case 0: ;
         if (p(move._.pto)) { return 1; }
         __uint128_t jspan = state->pieces[ps(black, 0x0)]
            - (state->pieces[ps(red, 0x0)] << 1);
         return !(jspan & FMASK[from] & ~state->pieces[empty]);
      case 1: {
         int32_t high = from > to ? from : to;
         int32_t low = from > to ? to : from;
         __uint128_t jspan = PMASK[high] - PMASK[low + 1];
         if (high - low >= d1r) { jspan = jspan & FMASK[high]; }
         return !(jspan & ~state->pieces[empty]); }
      case 2: ;
         int64_t diff = abs(from - to);
         int64_t offset = (diff == d1n2w) ? d1n1w
            : (diff == d2n1w) ? d1n1w : d1n1e;
         offset = from > to ? offset : -offset;
         return state->board[to + offset] == empty;
      case 3: ;
         int32_t high = from > to ? from : to;
         int32_t low = from > to ? to : from;
         int64_t count = move._.pto != empty;
         __uint128_t pspan = PMASK[high] - PMASK[low + 1];
         if (high - low >= d1r) { pspan = pspan & FMASK[high]; }
         pspan = pspan & ~state->pieces[empty];
         return popcnt(pspan) == count;
      case 4: return 1;
      case 5: return state->board[(from + to) / 2] == empty;
      case 6: return 1;
   }

   __builtin_unreachable();
}

uint32_t is_legal(struct transient_t* state, union move_t move) {
   if (!is_valid(state, move)) { return 0; }

   advance_board(move, state);
   uint32_t legal = !in_check(state, state->side);
   retract_board(move, state);

   return legal;
}

uint32_t is_repetition(struct transient_t* state) {
   int32_t step = trunk.ply + state->ply;
   return step > 4 ? *((uint64_t*)(state->hashes + step - 1))
      == *((uint64_t*)(state->hashes + step - 5)) : 0;
}
