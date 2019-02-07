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
   uint32_t active;
   uint32_t flags;
} option_t;

/*!
 * parse_opts
 * @ parse command line options
 */

int parse_opts(int argc, char const* argv[], uint32_t nopts, option_t** opts);

#endif /* OPTIONS_H */