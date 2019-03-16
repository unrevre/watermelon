#ifndef FEN_H
#define FEN_H

extern char fen_side[2];
extern char fen_char[16];

/*!
 * reset_fen
 * @ set game status with fen string
 * # (almost) no sanity checks
 */

void reset_fen(const char* fen_str);

#endif /* FEN_H */
