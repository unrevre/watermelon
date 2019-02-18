#include "state.h"

#include "fen.h"
#include "generate.h"
#include "inlines.h"
#include "magics.h"
#include "masks.h"
#include "memory.h"
#include "structs.h"

#include <stdlib.h>
#include <string.h>

state_t game __attribute__((aligned(64)));

uint32_t board[128] __attribute__((aligned(64)));

uint32_t PSHASH[15][128] __attribute__((aligned(64)));
uint32_t MVHASH;

transient_t state;

void init_hashes(void) {
   srand(0x91);

   for (int64_t i = 0x0; i != empty; ++i)
      for (int64_t j = 0; j != 90; ++j)
         PSHASH[i][j] = rand();
   for (int64_t i = 0; i != 128; ++i)
      PSHASH[empty][i] = 0x0;

   for (int64_t i = 0x0; i != empty; ++i)
      for (__uint128_t p = game.pieces[i]; p; p &= p - 1)
         state.hash ^= PSHASH[i][bsf(p)];

   state.hash ^= rand();
   htable[0] = state.hash;

   MVHASH = rand();
}

void init_state(const char* fen) {
   game = (state_t){ {0}, {0} };
   state = (transient_t){ 0, 0, 0, 0 };

   init_tables();
   init_masks();
   init_fen(fen);
   init_hashes();
}

void advance(move_t move) {
   ++state.ply;
   state.side = o(state.side);

   state.hash ^= PSHASH[move._.pfrom][move._.from];
   state.hash ^= PSHASH[move._.pfrom][move._.to];
   state.hash ^= PSHASH[move._.pto][move._.to];
   state.hash ^= MVHASH;

   htable[++state.step] = state.hash;

   game.pieces[move._.pfrom] ^= PMASK[move._.from] ^ PMASK[move._.to];
   game.pieces[move._.pto] ^= PMASK[move._.to];
   game.pieces[empty] ^= PMASK[move._.from];

   uint32_t s = s(move._.pfrom);
   game.occupancy[s] ^= PMASK[move._.from] ^ PMASK[move._.to];
   game.occupancy[!s] ^= game.occupancy[0] & game.occupancy[1];

   board[move._.from] = empty;
   board[move._.to] = move._.pfrom;
}

void retract(move_t move) {
   --state.ply;
   state.side = o(state.side);

   state.hash ^= PSHASH[move._.pfrom][move._.from];
   state.hash ^= PSHASH[move._.pfrom][move._.to];
   state.hash ^= PSHASH[move._.pto][move._.to];
   state.hash ^= MVHASH;

   --state.step;

   game.pieces[move._.pfrom] ^= PMASK[move._.from] ^ PMASK[move._.to];
   game.pieces[move._.pto] ^= PMASK[move._.to];
   game.pieces[empty] ^= PMASK[move._.from];

   uint32_t s = s(move._.pfrom);
   game.occupancy[s] ^= PMASK[move._.from] ^ PMASK[move._.to];
   game.occupancy[!s] ^= (game.pieces[empty] & PMASK[move._.to])
      ^ PMASK[move._.to];

   board[move._.from] = move._.pfrom;
   board[move._.to] = move._.pto;
}

void advance_with_history(move_t move) {
   uint32_t step = state.step;
   move_t future = history[step];
   if (future.bits && move.bits != future.bits)
      while (history[++step].bits)
         history[step] = (move_t){0};

   history[state.step] = move;

   advance(move);
   state.ply = 0;
}

void retract_with_history(move_t move) {
   retract(move);
   state.ply = 0;
}

void undo_history() {
   if (state.step) { retract_with_history(history[state.step - 1]); }
}

void redo_history() {
   if (history[state.step].bits) { advance_with_history(history[state.step]); }
}

uint32_t is_legal(move_t move) {
   if (!is_valid(move, state.side)) { return 0; }

   advance(move);
   uint32_t legal = !in_check(o(state.side));
   retract(move);

   return legal;
}

uint32_t is_repetition() {
   return state.step > 4
      && (htable[state.step] == htable[state.step - 0x4])
      && (htable[state.step - 0x1] == htable[state.step - 0x5]);
}

uint32_t is_index_movable(uint32_t index) {
   return (board[index] != empty && s(board[index]) == state.side);
}
