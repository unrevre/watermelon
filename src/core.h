#ifndef CORE_H
#define CORE_H

#include "timer.h"

#include <stdint.h>

/*!
 * search_t
 * @ search status information
 */

typedef struct {
   wmclock_t* clock;
   uint64_t nodes;
   uint64_t qnodes;
   uint64_t tthits;
} search_t;

extern search_t search;

/*!
 * smp_search
 * @ main search function
 */

void smp_search(int32_t depth);

#endif /* CORE_H */
