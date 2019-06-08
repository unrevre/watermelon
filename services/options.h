#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdint.h>

/*!
 * option_t
 * @ options struct
 */

struct option_t {
   char const* short_opt;
   char const* long_opt;
   char const* opt_str;
   int64_t active;
   int64_t flags;
};

/*!
 * parse_opts
 * @ parse command line options
 */

int parse_opts(int64_t argc, char const* argv[], int64_t nopts,
               struct option_t* opts);

#endif /* OPTIONS_H */
