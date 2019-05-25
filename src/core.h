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
   uint32_t target;
   uint32_t depth;
   uint64_t nodes;
   uint64_t qnodes;
   uint64_t tthits;
} search_t;

extern search_t search;

/*!
 * initialise
 * @ initialise global state variables
 */

void initialise(const char* fen);

/*!
 * terminate
 * @ destroy global state variables
 */

void terminate(void);

/*!
 * set_limit
 * @ set move time limit
 */

void set_limit(double limit);

/*!
 * smp_search
 * @ main search function
 */

void smp_search(int32_t depth);

/*!
 * smp_depth
 * @ return depth for next iteration of search
 */

int32_t smp_depth(void);

#endif /* CORE_H */
