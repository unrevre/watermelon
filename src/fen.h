#ifndef FEN_H
#define FEN_H

extern const char fen_side[2];
extern const char fen_char[15];

/*!
 * reset_fen
 * @ set game status with fen string
 * # (almost) no sanity checks
 */

void reset_fen(const char* fen_str);

#endif /* FEN_H */
