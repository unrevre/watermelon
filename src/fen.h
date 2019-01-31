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

void init_fen(const char* fen_str);

/*!
 * info_fen
 * @ get current game state as fen string
 */

char* info_fen(void);

#endif /* FEN_H */
