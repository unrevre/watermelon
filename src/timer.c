#include "timer.h"

void tick(timer_t* clock) {
    clock->ref = time(NULL);
}

int64_t drop(timer_t* clock) {
    return difftime(time(NULL), clock->ref) > clock->limit;
}