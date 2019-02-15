#include "options.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void free_options(option_t** options, uint32_t nopts) {
   for (uint32_t i = 0; i < nopts; ++i)
      free(options[i]);

   free(options);
}

/*!
 * sink
 * @ (static) shift arguments forward
 */

static void sink(uint32_t start, uint32_t end, char const* argv[]) {
   char const* a = argv[start];
   for (uint32_t i = start; i > end; --i)
      argv[i] = argv[i - 1];
   argv[end] = a;
}

int parse_opts(int argc, char const* argv[], uint32_t nopts, option_t** opts) {
   int positional = 1;

   for (int i = 1; i < argc; ++i) {
      if (argv[i][0] != '-' || argv[i][1] == '\0') {
         sink(i, positional++, argv);
         continue;
      }

      uint32_t type = argv[i][1] == '-';
      for (uint32_t j = 0; j < nopts; ++j) {
         if (opts[j]->active) { continue; }

         if (!strcmp(argv[i] + 1 + type, type ?
               opts[j]->long_opt : opts[j]->short_opt)) {
            opts[j]->active = 1;
            if (opts[j]->flags & 0x1)
               opts[j]->opt_str = argv[++i];

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
