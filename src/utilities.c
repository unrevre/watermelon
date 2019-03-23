#include "utilities.h"

#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

char** slice(char* string) {
   int64_t size = 8;
   char** slices = malloc(size * sizeof(char*));

   int64_t state = 0;
   int64_t count = 0;

   for (char* p = string; *p != '\0'; ++p) {
      if (state == 0) {
         if (isspace(*p)) {
            continue; }

         if (*p == '"') {
            state = 1;
            slices[count++] = p + 1;
         } else {
            state = 2;
            slices[count++] = p;
         }

         if (count == size) {
            size = size * 2;
            slices = realloc(slices, size);
         }
      } else {
         if (*p == '\0' || (state == 1 && *p == '"')
               || (state == 2 && isspace(*p))) {
            state = 0;
            *p = '\0';
         }
      }
   }

   slices = realloc(slices, count + 1);
   slices[count] = 0;

   return slices;
}

char** append(char** slices, char* string) {
   int64_t count = 0;
   for (char** t = slices; *t != 0; ++t, ++count);

   slices = realloc(slices, count + 2);
   slices[count++] = string;
   slices[count] = 0;

   return slices;
}
