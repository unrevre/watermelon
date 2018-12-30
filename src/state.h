#ifndef STATE_H
#define STATE_H

#include "structs.h"

extern state_t GAME __attribute__((aligned(64)));;

extern uint32_t board[90];

extern uint32_t hashes[15][90];
extern uint32_t hash_move;
extern uint32_t hash_state;

void init_hashes(void);

#endif /* STATE_H */
