#ifndef PERF_H
#define PERF_H

#include "generate.h"
#include "structs.h"

/*!
 * perft
 * = number of legal moves
 */

uint64_t perft(uint32_t depth, uint32_t side);

#endif /* PERF_H */
