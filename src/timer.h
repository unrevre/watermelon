#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <time.h>

/*!
 * wmclock_t
 * @ struct for time management
 */

typedef struct {
   double limit;
   time_t ref;
} wmclock_t;

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

#endif /* TIMER_H */
