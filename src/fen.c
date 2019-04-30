#include "fen.h"

#include "magics.h"
#include "masks.h"
#include "state.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char fen_side[2] = { 'r', 'b' };
char fen_char[16] = "KkRrNnCcAaBbPp ";

char fen_default[62] = "rnbakabnr/9/1c5c1/p1p1p1p1p/9/"
                       "9/P1P1P1P1P/1C5C1/9/RNBAKABNR r";

void reset_fen(const char* fen_str) {
   char schar;
   char fstr[100] = {0};

   if (!fen_str) { fen_str = fen_default; }
   sscanf(fen_str, "%s %c", fstr, &schar);

   for (int64_t i = 0; i != PIECES; ++i)
      game.pieces[i] = 0x0;
   for (int64_t i = 0; i != POINTS; ++i)
      board[i] = empty;

   char* fstr_p = fstr;
   for (int64_t i = 81; *fstr_p; ++fstr_p, ++i) {
      int32_t piece = -1;

      switch (*fstr_p) {
         case '1': case '2': case '3': case '4': case '5':
         case '6': case '7': case '8': case '9':
            i += *fstr_p - '1'; break;
         case 'k': case 'K': piece = 0x0; break;
         case 'r': case 'R': piece = 0x1; break;
         case 'n': case 'N': piece = 0x2; break;
         case 'c': case 'C': piece = 0x3; break;
         case 'a': case 'A': piece = 0x4; break;
         case 'b': case 'B': piece = 0x5; break;
         case 'p': case 'P': piece = 0x6; break;
         case '/': i -= 19; break;
      }

      if (piece != -1) {
         int64_t side = (*fstr_p > 'Z');
         board[i] = ps(side, piece);
      }
   }

   game.occupancy[0] = 0x0;
   game.occupancy[1] = 0x0;

   for (int64_t i = 0; i != POINTS; ++i) {
      uint32_t piece = board[i];
      game.pieces[piece] |= PMASK[i];
      if (piece != empty) {
         game.occupancy[s(piece)] |= PMASK[i]; }
   }

   game.pieces[empty] = ~(game.occupancy[0] | game.occupancy[1]);

   trunk.side = (schar == 'b') ? black : red;
}

int64_t side_from(char const* fen_str) {
   if (!fen_str) { fen_str = fen_default; }

   char schar;
   sscanf(fen_str, "%*s %c", &schar);
   return schar == 'b' ? black : red;
}
