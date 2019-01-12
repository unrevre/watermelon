#ifndef FEN_H
#define FEN_H

#include <stdint.h>

extern char fen_side[2];

extern char fen_rep[16];

/*!
 * init_fen
 * @ initialise game with fen string
 * # (almost) no sanity checks
 */

uint32_t init_fen(const char* fen_str);

/*!
 * dump_fen
 * @ dump current game state as fen string
 */

char* dump_fen(uint32_t side);

#endif /* FEN_H */
