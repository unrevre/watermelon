#include "magics.h"
#include "utilities.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

pid_t childpid[2] = {0};
int fd[2][2] = {{0, 1}};

char const sides[2] = "rb";

void vet(char* token, char const* tag);
void taste(int64_t side, char* buffer);
void serve(char* args[], int64_t side);

int main(int argc, char* argv[]) {
   if (argc != 3 && argc != 4) {
      printf("usage: %s [red] [black] [fen]\n", argv[0]);
      return -1;
   }

   char* tokens[8];

   char* fen = (argc == 3) ? 0 : argv[3];
   serve(append(slice(tokens, argv[1]), fen), red);
   serve(append(slice(tokens, argv[2]), fen), black);

   printf("served!\n");
   printf("  pids: %i, %i\n\n", childpid[0], childpid[1]);

   char buffer[256];
   sprintf(buffer, "\n");

   int64_t side = red;
   for (int64_t i = 0; i < STEPLIMIT; ++i, side = o(side)) {
      taste(side, buffer);

      slice(tokens, buffer);
      vet(tokens[0], "move");

      sprintf(buffer, "move %s %s\n", tokens[1] , tokens[2]);
      printf("[%c] %s", sides[side], buffer);
   }

   printf("move limit exceeded\n");

   return -1;
}

void vet(char* token, char const* tag) {
   if (!token || strcmp(token, tag)) {
      printf("fatal: invalid read\n"); exit(1); }
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

   int32_t status;
   if (waitpid(-1, &status, WNOHANG)) {
      printf("%c loss\n", sides[side]);
      exit(side);
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
}
