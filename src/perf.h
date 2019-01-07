#ifndef PERF_H
#define PERF_H

#include "structs.h"

#include <stdint.h>

/*!
 * perft
 * @ number of legal moves
 */

uint64_t perft(uint32_t depth, uint32_t side);

/*!
 * perft_capture
 * @ number of legal capture moves
 */

uint64_t perft_capture(uint32_t depth, uint32_t side);

/*!
 * trace
 * @ trace principal variation
 */

void trace(uint32_t side);

#endif /* PERF_H */
