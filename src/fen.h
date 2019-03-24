#ifndef FEN_H
#define FEN_H

#include <stdint.h>

extern char fen_side[2];
extern char fen_char[16];

/*!
 * reset_fen
 * @ set game status with fen string
 * # (almost) no sanity checks
 */

void reset_fen(const char* fen_str);

/*!
 * side_from
 * @ parse fen string for side to move
 */

int64_t side_from(char const* fen_str);

#endif /* FEN_H */
