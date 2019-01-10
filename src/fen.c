#include "fen.h"

#include "masks.h"
#include "state.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char fen_rep[15] = {
   'K', 'R', 'N', 'C', 'A', 'B', 'P', ' ',
   'k', 'r', 'n', 'c', 'a', 'b', 'p'
};

void init_fen(const char* fen_str) {
   char* lines[10];
   for (uint32_t i = 0; i < 10; ++i)
      lines[i] = (char*)malloc(9 * sizeof(char));

   sscanf(fen_str,
      "%[A-Za-z0-9]/%[A-Za-z0-9]/%[A-Za-z0-9]/"
      "%[A-Za-z0-9]/%[A-Za-z0-9]/%[A-Za-z0-9]/"
      "%[A-Za-z0-9]/%[A-Za-z0-9]/%[A-Za-z0-9]/%[A-Za-z0-9]",
      lines[9], lines[8], lines[7], lines[6], lines[5],
      lines[4], lines[3], lines[2], lines[1], lines[0]);

   char* fstr_cat = (char*)malloc(90 * sizeof(char));
   for (uint32_t i = 0; i < 10; ++i)
      strcat(fstr_cat, lines[i]);

   game.occupancy[0] = 0x0;
   game.occupancy[1] = 0x0;
   game.empty = 0x0;

   for (uint32_t i = 0; i < 15; ++i)
      game.pieces[i] = 0x0;

   for (uint32_t i = 0; i < 90; ++i)
      board[i] = 0x7;

   char* fstr_p = fstr_cat;
   for (uint32_t i = 0; *fstr_p; ++fstr_p, ++i) {
      uint32_t side = (*fstr_p > 'Z') << 3;
      switch (*fstr_p) {
         case '1': case '2': case '3': case '4': case '5':
         case '6': case '7': case '8': case '9':
            i += *fstr_p - '1';
            break;
         case 'k': case 'K':
            game.pieces[side] |= PMASK[i];
            board[i] = side;
            break;
         case 'r': case 'R':
            game.pieces[side + 1] |= PMASK[i];
            board[i] = side + 1;
            break;
         case 'n': case 'N':
            game.pieces[side + 2] |= PMASK[i];
            board[i] = side + 2;
            break;
         case 'c': case 'C':
            game.pieces[side + 3] |= PMASK[i];
            board[i] = side + 3;
            break;
         case 'a': case 'A':
            game.pieces[side + 4] |= PMASK[i];
            board[i] = side + 4;
            break;
         case 'b': case 'B':
            game.pieces[side + 5] |= PMASK[i];
            board[i] = side + 5;
            break;
         case 'p': case 'P':
            game.pieces[side + 6] |= PMASK[i];
            board[i] = side + 6;
            break;
         default:
            printf("invalid char in FEN str\n");
      }
   }

   for (uint32_t i = 0; i < 7; ++i) {
      game.occupancy[0] |= game.pieces[i];
      game.occupancy[1] |= game.pieces[i + 8];
   }
   game.empty = BMASK ^ (game.occupancy[0] | game.occupancy[1]);

   for (uint32_t i = 0; i < 10; ++i)
      free(lines[i]);
   free(fstr_cat);
}

char* info_fen(void) {
   char* fen_str = (char*)calloc(100, sizeof(char));

   uint32_t f = 0;
   for (uint32_t i = 0; i < 10; ++i) {
      uint32_t s = 0;
      for (uint32_t j = 0; j < 9; ++j) {
         if (board[(9 - i) * 9 + j] == 0x7) {
            ++s;
         } else {
            if (s) { fen_str[f++] = '0' + s; s = 0; }
            fen_str[f++] = fen_rep[board[(9 - i) * 9 + j]];
         }
      }

      if (s) { fen_str[f++] = '0' + s; }
      fen_str[f++] = '/';
   }

   fen_str[--f] = '\0';

   return fen_str;
}
