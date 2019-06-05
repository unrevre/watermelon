#include "core.h"
#include "debug.h"
#include "interface.h"
#include "memory.h"
#include "options.h"
#include "state.h"

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

enum options {
   opt_afk,
   opt_curses,
   opt_depth,
   opt_once,
   opt_quiet,
   opt_side,
   opt_threads,
   opt_time,
   nopts
};

option_t* set_options(int64_t nopts);
int watermelon(option_t* options, char const* fen);

int main(int argc, char const* argv[]) {
   option_t* options = set_options(nopts);
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

int watermelon(option_t* options, char const* fen) {
   int64_t afk = options[opt_afk].active;
   int64_t curses = options[opt_curses].active;
   int32_t depth = atoi(options[opt_depth].opt_str);
   int64_t once = options[opt_once].active;
   int64_t quiet = options[opt_quiet].active;
   char const* side = options[opt_side].opt_str;
   int64_t threads = atoi(options[opt_threads].opt_str);
   double time = atof(options[opt_time].opt_str);

   int64_t idle[2] = {0, 0};
   if (afk || once) { idle[0] = 1; idle[1] = 1; }
   char const* sides[2] = { "red", "black" };
   if (!strcmp(side, sides[0])) { idle[0] = 1; }
   if (!strcmp(side, sides[1])) { idle[1] = 1; }

   free(options);

   initialise(fen);
   set_limit(time);
   set_threads(threads);

   interface_t* itf = malloc(sizeof(interface_t));
   init_interface(itf, set(ITF_CURSES, curses) | set(ITF_QUIET, quiet));

   move_t move;

   do {
      wmprint_state(itf);
      refresh_interface(itf);

      if (!idle[trunk.side] && !event_loop(itf)) { break; }

      smp_search(depth);

      move = move_for_state(&trunk);
      wmprint_search(itf, move);
   } while (!once && advance_if_legal(move));

   close_interface(itf);
   terminate();

   debug_printf("alpha-beta nodes | quiescence nodes |  hash table hits |\n");
   debug_printf("%16"PRIu64" | %16"PRIu64" | %16"PRIu64" |\n\n",
                nodes, qnodes, tthits);

   return 0;
}

option_t* set_options(int64_t nopts) {
   option_t* options = malloc(nopts * sizeof(option_t));

   options[opt_afk] = (option_t){ "a", "afk", 0, 0, 0 };
   options[opt_curses] = (option_t){ "c", "curses", 0, 0, 0 };
   options[opt_depth] = (option_t){ "d", "depth", "4", 0, 1 };
   options[opt_once] = (option_t){ "1", "once", 0, 0, 0 };
   options[opt_quiet] = (option_t) { "q", "quiet", 0, 0, 0 };
   options[opt_side] = (option_t){ "s", "side", "none", 0, 1 };
   options[opt_threads] = (option_t){ "j", "threads", "1", 0, 1 };
   options[opt_time] = (option_t){ "t", "time", "144", 0, 1 };

   return options;
}