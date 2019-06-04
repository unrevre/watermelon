#ifndef PERF_H
#define PERF_H

#include <stdint.h>

/*!
 * perft
 * @ number of legal moves
 */

int64_t perft(int32_t depth);

/*!
 * perft_capture
 * @ number of legal capture moves
 */

int64_t perft_capture(int32_t depth);

#endif /* PERF_H */
