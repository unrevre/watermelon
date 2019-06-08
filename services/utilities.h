#ifndef UTILITIES_H
#define UTILITIES_H

/*!
 * slice
 * @ slice string into tokens, respecting double quotes
 */

char** slice(char** slices, char* string);

/*!
 * append
 * @ append string to token list
 */

char** append(char** slices, char* string);

#endif /* UTILITIES_H */
