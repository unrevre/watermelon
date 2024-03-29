#include "core.h"
#include "debug.h"
#include "interface.h"
#include "options.h"
#include "structs.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef INFO
#include <inttypes.h>
#endif /* INFO */

enum options {
   opt_curses,
   opt_depth,
   opt_once,
   opt_quiet,
   opt_threads,
   opt_time,
   nopts
};

struct option_t* set_options(int64_t nopts);
int watermelon(struct option_t* options, char const* fen);

int main(int argc, char const* argv[]) {
   struct option_t* options = set_options(nopts);
   argc = parse_opts(argc, argv, nopts, options);

   switch (argc) {
      case 1:
         return watermelon(options, 0);
      case 2:
         return watermelon(options, argv[1]);
      default:
         printf("usage: %s [fen]\n", argv[0]);
         return 1;
   }
}

int watermelon(struct option_t* options, char const* fen) {
   int64_t curses = options[opt_curses].active;
   int32_t depth = atoi(options[opt_depth].opt_str);
   int64_t once = options[opt_once].active;
   int64_t quiet = options[opt_quiet].active;
   int64_t threads = atoi(options[opt_threads].opt_str);
   double time = atof(options[opt_time].opt_str);

   free(options);

   initialise(fen);
   settings(time, threads);

   struct interface_t* itf = malloc(sizeof(struct interface_t));
   init_interface(itf, set(ITF_CURSES, curses) | set(ITF_QUIET, quiet));

   union move_t move;

   do {
      if (!once && !event_loop(itf)) { break; }
      move = smp_search(depth);
   } while (update(itf, move) && !once);

   close_interface(itf);

   terminate();

   debug_printf("alpha-beta nodes | quiescence nodes |  hash table hits |\n");
   debug_printf("%16"PRIu64" | %16"PRIu64" | %16"PRIu64" |\n\n",
                nodes, qnodes, tthits);

   return 0;
}

struct option_t* set_options(int64_t nopts) {
   struct option_t* options = malloc(nopts * sizeof(struct option_t));

   options[opt_curses] = (struct option_t){ "c", "curses", 0, 0, 0 };
   options[opt_depth] = (struct option_t){ "d", "depth", "4", 0, 1 };
   options[opt_once] = (struct option_t){ "1", "once", 0, 0, 0 };
   options[opt_quiet] = (struct option_t) { "q", "quiet", 0, 0, 0 };
   options[opt_threads] = (struct option_t){ "j", "threads", "1", 0, 1 };
   options[opt_time] = (struct option_t){ "t", "time", "144", 0, 1 };

   return options;
}
