#include "utilities.h"

#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

char** slice(char* string) {
   int64_t size = 8;
   char** slices = malloc(size * sizeof(char*));

   char* marker;
   int64_t state = 0;
   int64_t count = 0;

   for (char* p = string; *p != '\0'; ++p) {
      if (state == 0) {
         if (isspace(*p)) {
            continue; }

         if (*p == '"') {
            state = 1;
            marker = p + 1;
         } else {
            state = 2;
            marker = p;
         }
      } else {
         if ((state == 1 && *p == '"') || (state == 2 && isspace(*p))) {
            int64_t len = p - marker;
            char* buffer = malloc(len + 1);
            memcpy(buffer, marker, len);
            buffer[len] = '\0';

            state = 0;
            slices[count++] = buffer;

            if (count == size) {
               size = size * 2;
               slices = realloc(slices, size);
            }
         }
      }
   }

   return slices;
}
