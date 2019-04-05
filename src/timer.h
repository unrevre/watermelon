#ifndef TIMER_H
#define TIMER_H

#include "structs.h"

#include <stdint.h>

/*!
 * tick
 * @ start timer
 */

void tick(timer_t* clock);

/*!
 * drop
 * @ check elapsed (real) time against limit
 */

int64_t drop(timer_t* clock);

#endif /* TIMER_H */
