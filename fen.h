#ifndef FEN_H
#define FEN_H

#include "structs.h"

/*!
 * init_fen
 * @ initialise game with fen string
 * # (almost) no sanity checks
 */

void init_fen(const char* fen_str);

#endif /* FEN_H */
