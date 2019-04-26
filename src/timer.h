#ifndef TIMER_H
#define TIMER_H

#include "structs.h"

#include <stdint.h>

#define TIME_RES  0xFFF

/*!
 * start
 * @ start timer
 */

void start(wmclock_t* clock);

/*!
 * tick
 * @ check elapsed (real) time against limit
 */

int64_t tick(wmclock_t* clock);

/*!
 * drop
 * @ force stop timer
 */

void drop(wmclock_t* clock);

#endif /* TIMER_H */
