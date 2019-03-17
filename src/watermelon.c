#include "debug.h"
#include "interface.h"
#include "memory.h"
#include "options.h"
#include "position.h"
#include "search.h"
#include "state.h"

#include <stdlib.h>
#include <time.h>

enum options { opt_depth, opt_once, opt_curses, opt_afk, opt_quiet, nopts };

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
   int32_t depth = atoi(options[opt_depth]->opt_str);
   int64_t once = options[opt_once]->active;
   int64_t mode = options[opt_curses]->active;
   int64_t afk = options[opt_afk]->active;
   int64_t quiet = options[opt_quiet]->active;

   afk = once ? 1 : afk;
   quiet = mode ? 0 : quiet;

   free_options(options, nopts);

   init_state(fen);

   interface_t* itf = malloc(sizeof(interface_t));;
   init_interface(itf, mode, quiet);

   move_t move;

   do {
      wmprint_state(itf);
      refresh_all(itf);

      if (!afk && !event_loop(itf)) { break; }

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
   debug_variable_values(3, nodes, qnodes, tthits);
   debug_printf("\n");

   return 0;
}

option_t** set_options(int64_t nopts) {
   option_t** options = malloc(nopts * sizeof(option_t*));

   for (int64_t i = 0; i < nopts; ++i)
      options[i] = calloc(1, sizeof(option_t));

   options[opt_depth]->short_opt = "d";
   options[opt_depth]->long_opt = "depth";
   options[opt_depth]->opt_str = "4";
   options[opt_depth]->flags = 0x1;

   options[opt_once]->short_opt = "1";
   options[opt_once]->long_opt = "once";

   options[opt_curses]->short_opt = "c";
   options[opt_curses]->long_opt = "curses";

   options[opt_afk]->short_opt = "a";
   options[opt_afk]->long_opt = "afk";

   options[opt_quiet]->short_opt = "q";
   options[opt_quiet]->long_opt = "quiet";

   return options;
}
