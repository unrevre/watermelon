#include "utilities.h"

#include <stdint.h>

char** slice(char** slices, char* string) {
   int64_t state = 0;
   int64_t count = 0;

   for (char* p = string; *p != '\0'; ++p) {
      if (state == 0) {
         if (*p == ' ' || *p == '\n') { continue; }

         int64_t quote = *p == '"';
         state = quote + 1;
         slices[count++] = p + quote;
      } else {
         if (*p == '\0' || (state == 2 && *p == '"')
               || (state == 1 && (*p == ' ' || *p == '\n'))) {
            state = 0;
            *p = '\0';
         }
      }
   }

   slices[count] = 0;

   return slices;
}

char** append(char** slices, char* string) {
   char** t = slices;
   for (; *t != 0; ++t);

   int64_t count = t - slices;
   slices[count++] = string;
   slices[count] = 0;

   return slices;
}
