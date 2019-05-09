#include "position.h"

#include "inlines.h"
#include "magics.h"
#include "masks.h"
#include "memory.h"
#include "state.h"

#include <stdlib.h>

uint32_t in_check(transient_t* state, int64_t side) {
   uint64_t index = bsf(state->game.pieces[ps(side, 0x0)]);

   __uint128_t pmask = PMASK[index];
   __uint128_t umask = UMASK[index];
   __uint128_t rmask = RMASK[index];
   __uint128_t fmask = FMASK[index];
   __uint128_t emask = state->game.pieces[empty];
   __uint128_t xmask = pmask | emask;

   __uint128_t pset = state->game.pieces[po(side, 0x3)];

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

   __uint128_t jset = state->game.pieces[po(side, 0x0)]
      | state->game.pieces[po(side, 0x1)];
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

   __uint128_t mset = ((_2n1w(pmask) | _2n1e(pmask)) & _1n(emask))
      | ((_2s1e(pmask) | _2s1w(pmask)) & _1s(emask))
      | ((_2e1s(pmask) | _2e1n(pmask)) & _1e(emask))
      | ((_2w1n(pmask) | _2w1s(pmask)) & _1w(emask));
   mset = mset & state->game.pieces[po(side, 0x2)];
   if (mset) { return 1; }

   __uint128_t zset = _1f(pmask, side) | _1e(pmask) | _1w(pmask);
   zset = zset & state->game.pieces[po(side, 0x6)];
   if (zset) { return 1; }

   return 0;
}

uint32_t is_valid(transient_t* state, move_t move, int64_t side) {
   if (!state->game.pieces[ps(side, 0x0)]) { return 0; }
   if (side != s(move._.pfrom)) { return 0; }

   int32_t from = move._.from;
   if (state->game.board[from] != move._.pfrom) { return 0; }

   int32_t to = move._.to;
   if (state->game.board[to] != move._.pto) { return 0; }

   switch (p(move._.pfrom)) {
      case 0: ;
         if (p(move._.pto)) { return 1; }
         __uint128_t jspan = state->game.pieces[ps(black, 0x0)]
            - (state->game.pieces[ps(red, 0x0)] << 1);
         return !(jspan & FMASK[from] & ~state->game.pieces[empty]);
      case 1: {
         int32_t high = from > to ? from : to;
         int32_t low = from > to ? to : from;
         __uint128_t jspan = PMASK[high] - (PMASK[low] << 1);
         if (high - low >= WIDTH) { jspan = jspan & FMASK[high]; }
         return !(jspan & ~state->game.pieces[empty]); }
      case 2: ;
         int64_t diff = abs(from - to);
         int64_t offset = (diff == (WIDTH - 2))
            ? (WIDTH - 1) : (diff == ((WIDTH << 1) - 1))
            ? (WIDTH - 1) : (WIDTH + 1);
         offset = from > to ? offset : -offset;
         return state->game.board[to + offset] == empty;
      case 3: ;
         int32_t high = from > to ? from : to;
         int32_t low = from > to ? to : from;
         int64_t count = move._.pto != empty;
         __uint128_t pspan = PMASK[high] - (PMASK[low] << 1);
         if (high - low >= WIDTH) { pspan = pspan & FMASK[high]; }
         pspan = pspan & ~state->game.pieces[empty];
         return popcnt(pspan) == count;
      case 4: return 1;
      case 5: return state->game.board[(from + to) / 2] == empty;
      case 6: return 1;
   }

   __builtin_unreachable();
}

uint32_t is_legal(transient_t* state, move_t move, int64_t side) {
   if (!is_valid(state, move, side)) { return 0; }

   advance_board(move, state);
   uint32_t legal = !in_check(state, side);
   retract_board(move, state);

   return legal;
}

uint32_t is_repetition(transient_t* state) {
   int32_t step = trunk.ply + state->ply;
   return step > 4 ? *((uint64_t*)(htable + step - 1))
      == *((uint64_t*)(htable + step - 5)) : 0;
}

uint32_t is_index_movable(int64_t index) {
   return (trunk.game.board[index] != empty
      && s(trunk.game.board[index]) == trunk.side);
}

move_t move_for_indices(uint32_t from, uint32_t to) {
   if (from >= POINTS || to >= POINTS) { return (move_t){0}; }
   if (is_index_movable(to)) { return (move_t){0}; }

   int64_t side = trunk.side;
   __uint128_t tpmask = PMASK[to];

   int32_t fdiff = (from - OFFSET) % WIDTH - (to - OFFSET) % WIDTH;
   int32_t fdabs = abs(fdiff);
   int32_t rdiff = (from - OFFSET) / WIDTH - (to - OFFSET) / WIDTH;
   int32_t rdabs = abs(rdiff);

   switch (p(trunk.game.board[from])) {
      case 0:
         if ((fdabs + rdabs != 1) || !(tpmask & JMASK[side]))
            return (move_t){0};
         break;
      case 1: case 3:
         if (fdabs && rdabs)
            return (move_t){0};
         break;
      case 2:
         if (!fdabs || !rdabs || (fdabs + rdabs != 3))
            return (move_t){0};
         break;
      case 4:
         if (fdabs != 1 || rdabs != 1 || !(tpmask & SMASK[side]))
            return (move_t){0};
         break;
      case 5:
         if (fdabs != 2 || rdabs != 2 || !(tpmask & XMASK[side]))
            return (move_t){0};
         break;
      case 6:
         if ((fdabs + rdabs != 1) || !(tpmask & ZMASK[side])
               || (rdiff == (side ? -1 : 1)))
            return (move_t){0};
         break;
   }

   return (move_t){ ._ = {
      from, to, trunk.game.board[from], trunk.game.board[to] } };
}
