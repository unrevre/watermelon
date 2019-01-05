#ifndef DEBUG_H
#define DEBUG_H

#include <stdint.h>

void display(__uint128_t bits);

void tree_node_exit(uint32_t ply, int32_t alpha, int32_t beta, int32_t score,
                    uint32_t side);

void tree_node_entry(uint32_t ply, int32_t alpha, int32_t beta, uint32_t side);

#endif /* DEBUG_H */
