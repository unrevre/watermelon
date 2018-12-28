#ifndef FEN_H
#define FEN_H

/*!
 * init_fen
 * @ initialise game with fen string
 * # (almost) no sanity checks
 */

void init_fen(const char* fen_str);

/*!
 * dump_fen
 * @ dump current game state as fen string
 */

char* dump_fen();

#endif /* FEN_H */
