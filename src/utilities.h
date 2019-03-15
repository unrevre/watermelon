#ifndef UTILITIES_H
#define UTILITIES_H

/*!
 * slice
 * @ slice string into tokens, respecting double quotes
 */

char** slice(char* string);

/*!
 * clean
 * @ free memory allocated to sliced string
 */

void clean(char** slices);

#endif /* UTILITIES_H */
