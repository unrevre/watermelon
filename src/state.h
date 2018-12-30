#ifndef STATE_H
#define STATE_H

#include "structs.h"

extern state_t GAME __attribute__((aligned(64)));;

extern uint32_t board[90];

extern uint32_t hashes[15][90];
extern uint32_t hash_move;
extern uint32_t hash_state;

extern ttentry_t TTABLE[0x1000000] __attribute__((aligned(64)));

void init_hashes(void);

void init_tables(void);

#endif /* STATE_H */
