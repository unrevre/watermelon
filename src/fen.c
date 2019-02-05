#include "fen.h"

#include "magics.h"
#include "masks.h"
#include "state.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

char fen_side[2] = {'r', 'b'};

char fen_rep[16] = "KkRrNnCcAaBbPp ";

void init_fen(const char* fen_str) {
   char* lines[10];
   for (uint32_t i = 0; i < 10; ++i)
      lines[i] = malloc(10 * sizeof(char));

   char schar;

   sscanf(fen_str,
      "%[A-Za-z0-9]/%[A-Za-z0-9]/%[A-Za-z0-9]/"
      "%[A-Za-z0-9]/%[A-Za-z0-9]/%[A-Za-z0-9]/"
      "%[A-Za-z0-9]/%[A-Za-z0-9]/%[A-Za-z0-9]/%[A-Za-z0-9] %c",
      lines[9], lines[8], lines[7], lines[6], lines[5],
      lines[4], lines[3], lines[2], lines[1], lines[0],
      &schar);

   char* fstr_cat = calloc(91, sizeof(char));
   for (uint32_t i = 0; i < 10; ++i)
      strcat(fstr_cat, lines[i]);

   for (uint32_t i = 0; i < 15; ++i)
      game.pieces[i] = 0x0;
   for (uint32_t i = 0; i < 90; ++i)
      board[i] = empty;

   char* fstr_p = fstr_cat;
   for (uint32_t i = 0; *fstr_p; ++fstr_p, ++i) {
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
         default: printf(" [fen] invalid char!\n");
      }

      if (piece > -1) {
         uint32_t side = (*fstr_p > 'Z');
         game.pieces[ps(side, piece)] |= PMASK[i];
         board[i] = ps(side, piece);
      }
   }

   game.occupancy[0] = 0x0;
   game.occupancy[1] = 0x0;

   for (uint32_t i = 0; i < 7; ++i) {
      game.occupancy[0] |= game.pieces[ps(0x0, i)];
      game.occupancy[1] |= game.pieces[po(0x0, i)];
   }

   game.pieces[empty] = ~(game.occupancy[0] | game.occupancy[1]);

   for (uint32_t i = 0; i < 10; ++i)
      free(lines[i]);
   free(fstr_cat);

   state.side = (schar == 'b') ? BSIDE : RSIDE;
}
