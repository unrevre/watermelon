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

   info_fen(info->buffer);
   printf("%s\n", info->buffer);
   union move_t move = smp_search(atoi(argv[1]));
   info_move(info->buffer, move);
   printf("%s\n\n", info->buffer);

   info_principal_variation(info->buffers, 0);
   for (char** pv = info->buffers; **pv; ++pv)
      printf("%s", *pv);
   printf("\n");

   free(info);

   return 0;
}
