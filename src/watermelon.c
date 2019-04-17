#include "debug.h"
#include "interface.h"
#include "memory.h"
#include "options.h"
#include "position.h"
#include "search.h"
#include "state.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

enum options {
   opt_afk,
   opt_curses,
   opt_depth,
   opt_once,
   opt_quiet,
   opt_side,
   opt_time,
   nopts
};

int watermelon(option_t** options, char const* fen);

int main(int argc, char const* argv[]) {
   option_t** options = set_options(nopts);

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

int watermelon(option_t** options, char const* fen) {
   int64_t afk = options[opt_afk]->active;
   int64_t curses = options[opt_curses]->active;
   int32_t depth = atoi(options[opt_depth]->opt_str);
   int64_t once = options[opt_once]->active;
   int64_t quiet = options[opt_quiet]->active;
   char const* side = options[opt_side]->opt_str;
   double time = atof(options[opt_time]->opt_str);

   int64_t idle[2] = {0, 0};
   if (afk || once) { idle[0] = 1; idle[1] = 1; }
   static char const* sides[2] = { "red", "black" };
   if (!strcmp(side, sides[0])) { idle[0] = 1; }
   if (!strcmp(side, sides[1])) { idle[1] = 1; }

   free_options(options, nopts);

   init_state(fen);
   set_timer(time);

   interface_t* itf = malloc(sizeof(interface_t));
   init_interface(itf, set_interface(ITF_CURSES, curses)
      | set_interface(ITF_QUIET, quiet));

   move_t move;

   do {
      wmprint_state(itf);
      refresh_all(itf);

      if (!idle[trunk.side] && !event_loop(itf)) { break; }

      clock_t cpu_time = clock();
      move = iter_dfs(depth);
      cpu_time = clock() - cpu_time;

      wmprint_info(itf, "cpu_time: %fs\n\n", (float)cpu_time / CLOCKS_PER_SEC);

      wmprint_search(itf, move);
   } while (!once && is_legal(move) && (advance_history(move),
                                        advance_game(move), 1));

   close_interface(itf);

   debug_variable_headers(3,
      "alpha-beta nodes", "quiescence nodes", "hash table hits");
   debug_variable_values(3, search.nodes, search.qnodes, search.tthits);
   debug_printf("\n");

   return 0;
}

option_t** set_options(int64_t nopts) {
   option_t** options = malloc(nopts * sizeof(option_t*));
   for (int64_t i = 0; i < nopts; ++i)
      options[i] = calloc(1, sizeof(option_t));

   options[opt_afk]->short_opt = "a";
   options[opt_afk]->long_opt = "afk";

   options[opt_curses]->short_opt = "c";
   options[opt_curses]->long_opt = "curses";

   options[opt_depth]->short_opt = "d";
   options[opt_depth]->long_opt = "depth";
   options[opt_depth]->opt_str = "4";
   options[opt_depth]->flags = 0x1;

   options[opt_once]->short_opt = "1";
   options[opt_once]->long_opt = "once";

   options[opt_quiet]->short_opt = "q";
   options[opt_quiet]->long_opt = "quiet";

   options[opt_side]->short_opt = "s";
   options[opt_side]->long_opt = "side";
   options[opt_side]->opt_str = "none";
   options[opt_side]->flags = 0x1;

   options[opt_time]->short_opt = "t";
   options[opt_time]->long_opt = "time";
   options[opt_time]->opt_str = "144";
   options[opt_time]->flags = 0x1;

   return options;
}
