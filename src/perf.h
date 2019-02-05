#ifndef PERF_H
#define PERF_H

#include <stdint.h>

/*!
 * perft
 * @ number of legal moves
 */

uint64_t perft(uint32_t depth);

/*!
 * perft_capture
 * @ number of legal capture moves
 */

uint64_t perft_capture(uint32_t depth);

#endif /* PERF_H */
