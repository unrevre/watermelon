#include "timer.h"

void start(timer_t* clock) {
    clock->ref = time(NULL);
}

int64_t tick(timer_t* clock) {
    if (clock->limit < 0) { return 0; }

    clock->status = difftime(time(NULL), clock->ref) > clock->limit;
    return clock->status;
}

void drop(timer_t* clock) {
    clock->status = 1;
}
