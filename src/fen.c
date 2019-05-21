#include "fen.h"

#include "magics.h"
#include "masks.h"
#include "state.h"

#include <stdio.h>

const char fen_side[2] = { 'r', 'b' };
const char fen_char[15] = "KkRrNnCcAaBbPp ";

const char* fen_default = "rnbakabnr/9/1c5c1/p1p1p1p1p/9/"
                          "9/P1P1P1P1P/1C5C1/9/RNBAKABNR r";

void reset_fen(const char* fen_str) {
   char schar;
   char fstr[100] = {0};

   if (!fen_str) { fen_str = fen_default; }
   sscanf(fen_str, "%s %c", fstr, &schar);

   trunk.side = (schar == 'b') ? black : red;

   for (int64_t i = 0; i != PIECES; ++i)
      trunk.pieces[i] = 0x0;
   for (int64_t i = 0; i != 2; ++i)
      trunk.occupancy[i] = 0x0;
   for (int64_t i = 0; i != POINTS; ++i)
      trunk.board[i] = empty;

   char* fstr_p = fstr;
   for (int64_t i = index_for(0, HEIGHT - 1); *fstr_p; ++fstr_p, ++i) {
      int64_t p = 0;

      switch (*fstr_p) {
         case '/':
            i = i - FILES - WIDTH - 1;
            break;
         case '1': case '2': case '3': case '4': case '5':
         case '6': case '7': case '8': case '9':
            i += *fstr_p - '1';
            break;
         case 'p': case 'P': ++p;
         case 'b': case 'B': ++p;
         case 'a': case 'A': ++p;
         case 'c': case 'C': ++p;
         case 'n': case 'N': ++p;
         case 'r': case 'R': ++p;
         case 'k': case 'K': ;
            int64_t side = *fstr_p > 'Z';
            uint32_t piece = ps(side, p);
            trunk.pieces[piece] |= PMASK[i];
            trunk.occupancy[side] |= PMASK[i];
            trunk.board[i] = piece;
      }
   }

   trunk.pieces[empty] = ~(trunk.occupancy[0] | trunk.occupancy[1]);
}

int64_t side_from(char const* fen_str) {
   if (!fen_str) { fen_str = fen_default; }

   char schar;
   sscanf(fen_str, "%*s %c", &schar);
   return schar == 'b' ? black : red;
}
