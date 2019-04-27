#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdint.h>

/*!
 * option_t
 * @ options struct
 */

typedef struct {
   char const* short_opt;
   char const* long_opt;
   char const* opt_str;
   int64_t active;
   int64_t flags;
} option_t;

/*!
 * parse_opts
 * @ parse command line options
 */

int parse_opts(int argc, char const* argv[], int64_t nopts, option_t* opts);

#endif /* OPTIONS_H */
