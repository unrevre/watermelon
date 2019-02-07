#include "debug.h"
#include "options.h"
#include "search.h"
#include "state.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

enum options { opt_depth, nopts };
option_t** set_options(uint32_t nopts);
void free_options(option_t** options, uint32_t nopts);

int watermelon(option_t** options, char const* fen);

int main(int argc, char const* argv[]) {
   option_t** options = set_options(nopts);

   argc = parse_opts(argc, argv, nopts, options);

   switch (argc) {
      case 1:
         return watermelon(options,
            "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR r");
      case 2:
         return watermelon(options, argv[1]);
      default:
         printf("usage: %s [fen]\n", argv[0]);
         return 1;
   }
}

int watermelon(option_t** options, char const* fen) {
   uint32_t depth = atoi(options[opt_depth]->opt_str);

   free_options(options, nopts);

   init_state(fen);

   debug_t info;
   init_debug(&info);

   printf("%s\n", info_fen(&info));

   clock_t cpu_time = clock();
   move_t move = iter_dfs(depth);
   cpu_time = clock() - cpu_time;

   printf("cpu_time: %fs\n\n", (float)cpu_time / CLOCKS_PER_SEC);

   printf("%s\n\n", info_game_state(&info));
   printf("%s\n\n", info_move(&info, move));
   printf("%s\n", info_principal_variation(&info));

   free_debug(&info);

   debug_variable_headers(3,
      "alpha-beta nodes", "quiescence nodes", "hash table hits");
   debug_variable_values(3, nodes, qnodes, tthits);
   printf("\n");

   return 0;
}

option_t** set_options(uint32_t nopts) {
   option_t** options = malloc(nopts * sizeof(option_t*));

   for (uint32_t i = 0; i < nopts; ++i)
      options[i] = calloc(1, sizeof(option_t));

   options[opt_depth]->short_opt = "d";
   options[opt_depth]->long_opt = "depth";
   options[opt_depth]->opt_str = "4";
   options[opt_depth]->flags = 0x1;

   return options;
}

void free_options(option_t** options, uint32_t nopts) {
   for (uint32_t i = 0; i < nopts; ++i)
      free(options[i]);

   free(options);
}
