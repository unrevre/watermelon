#include "position.h"

#include "inlines.h"
#include "magics.h"
#include "masks.h"
#include "memory.h"
#include "state.h"

uint32_t in_check(int32_t side) {
   uint64_t index = bsf_branchless(game.pieces[ps(side, 0x0)]);

   __uint128_t pmask = PMASK[index];
   __uint128_t emask = game.pieces[empty];
   __uint128_t xmask = pmask | emask;

   __uint128_t jset = game.pieces[po(side, 0x0)] | game.pieces[po(side, 0x1)];
   __uint128_t jxmask = ~(jset | xmask);

   __uint128_t jrset = jset & RMASK[index];
   for (; jrset; jrset &= jrset - 1) {
      __uint128_t range = jrset ^ -jrset ^ UMASK[index];
      range = range & jxmask & RMASK[index];
      if (!range) { return 1; }
   }

   __uint128_t jfset = jset & FMASK[index];
   for (; jfset; jfset &= jfset - 1) {
      __uint128_t range = jfset ^ -jfset ^ UMASK[index];
      range = range & jxmask & FMASK[index];
      if (!range) { return 1; }
   }

   __uint128_t pset = game.pieces[po(side, 0x3)];

   __uint128_t prset = pset & RMASK[index];
   for (; prset; prset &= prset - 1) {
      __uint128_t range = prset ^ -prset ^ UMASK[index];
      __uint128_t lsb = prset & -prset;
      __uint128_t pxmask = ~(lsb | xmask);
      range = range & pxmask & RMASK[index];
      if (popcnt(range) == 1) { return 1; }
   }

   __uint128_t pfset = pset & FMASK[index];
   for (; pfset; pfset &= pfset - 1) {
      __uint128_t range = pfset ^ -pfset ^ UMASK[index];
      __uint128_t lsb = pfset & -pfset;
      __uint128_t pxmask = ~(lsb | xmask);
      range = range & pxmask & FMASK[index];
      if (popcnt(range) == 1) { return 1; }
   }

   __uint128_t mset = ((pmask << 0x11 | pmask << 0x13) & emask << 0x9)
      | ((pmask >> 0x11 | pmask >> 0x13) & emask >> 0x9)
      | ((pmask >> 0x07 | pmask << 0x0b) & emask << 0x1)
      | ((pmask << 0x07 | pmask >> 0x0b) & emask >> 0x1);
   mset = mset & game.pieces[po(side, 0x2)];
   if (mset) { return 1; }

   __uint128_t zset = (pmask << 9) >> (18 * side) | pmask << 1 | pmask >> 1;
   zset = zset & game.pieces[po(side, 0x6)];
   if (zset) { return 1; }

   return 0;
}

uint32_t is_valid(move_t move, int32_t side) {
   if (side != s(move._.pfrom)) { return 0; }

   uint32_t from = move._.from;
   if (board[from] != move._.pfrom) { return 0; }

   uint32_t to = move._.to;
   if (board[to] != move._.pto) { return 0; }

   switch (p(move._.pfrom)) {
      case 0: {
         if (p(move._.pto)) { return 1; }
         __uint128_t jspan = game.pieces[ps(black, 0x0)]
            - (game.pieces[ps(red, 0x0)] << 1);
         jspan &= FMASK[from] & ~game.pieces[empty];
         return !jspan; }
      case 1: {
         uint32_t high = from > to ? from : to;
         uint32_t low = from > to ? to : from;

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
         uint32_t high = from > to ? from : to;
         uint32_t low = from > to ? to : from;

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

uint32_t is_legal(move_t move) {
   if (!is_valid(move, state.side)) { return 0; }

   advance(move);
   uint32_t legal = !in_check(state.side ^ pass);
   retract(move);

   return legal;
}

uint32_t is_repetition() {
   return state.step > 4 ? *((uint64_t*)(htable + state.step - 1))
      == *((uint64_t*)(htable + state.step - 5)) : 0;
}

uint32_t is_index_movable(int32_t index) {
   return (board[index] != empty && s(board[index]) == state.side);
}

move_t move_for_indices(uint32_t from, uint32_t to) {
   if (is_index_movable(to)) { return (move_t){0}; }

   int32_t side = state.side;
   __uint128_t fpmask = PMASK[from];
   __uint128_t tpmask = PMASK[to];

   switch (p(board[from])) {
      case 0:
         if (!(tpmask & JMASK[side]
               & (fpmask << 9 | fpmask >> 9 | fpmask << 1 | fpmask >> 1)))
            return (move_t){0};
         break;
      case 1: case 3:
         if ((from % 9 != to % 9) && (from / 9 != to / 9))
            return (move_t){0};
         break;
      case 2:
         if (!(tpmask & BMASK
               & (((fpmask << 0x11 | fpmask >> 0x13) & FMASKN8)
               | ((fpmask >> 0x11 | fpmask << 0x13) & FMASKN0)
               | ((fpmask << 0x07 | fpmask >> 0x0b) & FMASKN78)
               | ((fpmask >> 0x07 | fpmask << 0x0b) & FMASKN01))))
            return (move_t){0};
         break;
      case 4:
         if (!(tpmask & SMASK[side]
               & (fpmask << 8 | fpmask << 10 | fpmask >> 8 | fpmask >> 10)))
            return (move_t){0};
         break;
      case 5:
         if (!(tpmask & XMASK[side]
               & (fpmask << 16 | fpmask << 20 | fpmask >> 16 | fpmask >> 20)))
            return (move_t){0};
         break;
      case 6:
         if (!(tpmask & ZMASK[side] & ((fpmask << 9) >> (18 * side)
               | (fpmask << 1 & FMASKN0) | (fpmask >> 1 & FMASKN8))))
            return (move_t){0};
         break;
   }

   return (move_t){ ._ = { from, to, board[from], board[to] } };
}
