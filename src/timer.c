#include "timer.h"

void start(wmclock_t* clock) {
   clock->ref = time(NULL);
}

int64_t tick(wmclock_t* clock) {
   if (clock->limit < 0) { return 0; }

   clock->status = difftime(time(NULL), clock->ref) > clock->limit;
   return clock->status;
}
