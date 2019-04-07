#ifndef TIMER_H
#define TIMER_H

#include "structs.h"

#include <stdint.h>

/*!
 * start
 * @ start timer
 */

void start(timer_t* clock);

/*!
 * tick
 * @ check elapsed (real) time against limit
 */

int64_t tick(timer_t* clock);

/*!
 * drop
 * @ force stop timer
 */

void drop(timer_t* clock);

#endif /* TIMER_H */
