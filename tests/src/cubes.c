#include "../../src/fen.h"
#include "../../src/magics.h"
#include "../../src/utilities.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

pid_t childpid[2] = {0};
int fd[2][2] = {{0, 1}};

void taste(int64_t side, char* buffer);
void serve(char* args[], int64_t side);

int main(int argc, char* argv[]) {
   if (argc != 3 && argc != 4) {
      printf("usage: %s [red] [black] [fen]\n", argv[0]);
      return 1;
   }

   char* fen = (argc == 3) ? 0 : argv[3];
   serve(append(slice(argv[1]), fen), red);
   serve(append(slice(argv[2]), fen), black);

   printf("served!\n");
   printf("  pids: %i, %i\n\n", childpid[0], childpid[1]);

   char buffer[256];
   sprintf(buffer, "\n");

   int64_t side = side_from(argv[3]);
   for (int64_t i = 0; i < STEPLIMIT; ++i, side = o(side)) {
      taste(side, buffer);
      printf("[%c] %s", fen_side[side], buffer);
   }

   sprintf(buffer, "eval\n");
   taste(red, buffer);

   char** tokens = slice(buffer);
   int64_t norm = (tokens[1][0] == 'r') * atoi(tokens[2]);
   int64_t advantage = (norm > 0) - (0 > norm);
   switch (advantage) {
      case -1: printf("r adv\n"); break;
      case 0: printf("even\n"); break;
      case 1: printf("b adv\n"); break;
   }

   return 0;
}

void taste(int64_t side, char* buffer) {
   write(fd[side][1], buffer, strlen(buffer));
   write(fd[side][1], "next\n", 5);

   fd_set set;
   FD_ZERO(&set);
   FD_SET(fd[side][0], &set);

   select(fd[side][0] + 1, &set, NULL, NULL, NULL);
   int32_t nbytes = read(fd[side][0], buffer, 256);
   buffer[nbytes] = '\0';

   sleep(1);

   int32_t status;
   if (waitpid(-1, &status, WNOHANG)) {
      printf("%c loss\n", fen_side[side]);
      exit(1);
   }

   char** tokens = slice(buffer);
   if (tokens[0]) {
      if (strlen(tokens[0]) == 2 && tokens[0][1] == ':') {
         int32_t from = atoi(tokens[1]);
         int32_t to = atoi(tokens[3]);

         sprintf(buffer, "move %i %i\n", from , to);
      } else if (!strcmp(tokens[0], "eval:")) {
         sprintf(buffer, "eval %c %i\n", tokens[1][1], atoi(tokens[2]));
      }
   } else {
      printf("fatal: invalid read\n");
      exit(1);
   }
}

void serve(char* args[], int64_t side) {
   int32_t r_pipe[2];
   int32_t w_pipe[2];

   pipe(r_pipe);
   pipe(w_pipe);

   childpid[side] = fork();

   if (!childpid[side]) {
      dup2(r_pipe[1], 1);
      dup2(w_pipe[0], 0);

      close(r_pipe[0]);
      close(w_pipe[1]);

      execvp(args[0], args);
   } else {
      close(r_pipe[1]);
      close(w_pipe[0]);

      fd[side][0] = r_pipe[0];
      fd[side][1] = w_pipe[1];
   }

   free(args);
}
