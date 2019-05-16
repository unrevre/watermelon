#include "options.h"

#include <stdio.h>
#include <string.h>

/*!
 * sink
 * @ (static) shift arguments forward
 */

static void sink(int64_t start, int64_t end, char const* argv[]) {
   char const* a = argv[start];
   for (int64_t i = start; i > end; --i)
      argv[i] = argv[i - 1];
   argv[end] = a;
}

int parse_opts(int argc, char const* argv[], int64_t nopts, option_t* opts) {
   int positional = 1;

   for (int i = 1; i < argc; ++i) {
      if (argv[i][0] != '-' || argv[i][1] == '\0') {
         sink(i, positional++, argv);
         continue;
      }

      int64_t type = argv[i][1] == '-';
      for (int64_t j = 0; j < nopts; ++j) {
         if (opts[j].active) { continue; }

         if (!strcmp(argv[i] + 1 + type, type ?
               opts[j].long_opt : opts[j].short_opt)) {
            opts[j].active = 1;
            if (opts[j].flags & 0x1)
               opts[j].opt_str = argv[++i];

            type = 0xf;
            break;
         }
      }

      if (type != 0xf) {
         fprintf(stderr, "warning: unrecognised option: %s\n",
                 argv[i] + 1 + type);
         return -1;
      }
   }

   return positional;
}
