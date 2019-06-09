#include "core.h"
#include "debug.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
   if (argc != 3) {
      printf("usage: %s [depth] [fen]\n", argv[0]);
      return 1;
   }

   initialise(argv[2]);

   struct debug_t* info = malloc(sizeof(struct debug_t));
   init_debug(info);

   printf("%s\n", info_fen(info));
   union move_t move = smp_search(atoi(argv[1]));
   printf("%s\n\n", info_move(info, move));

   for (char** pv = info_principal_variation(info); **pv; ++pv)
      printf("%s", *pv);
   printf("\n");

   free_debug(info);

   terminate();

   return 0;
}
