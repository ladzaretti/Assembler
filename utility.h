#ifndef UTILITY_H
#define UTILITY_H
/*concatenate the given strings into a new string without changing the input.
input - first argument is the beginning, the second string is the ending.
return pointer to a new allocated string containing the result.*/
char *strcat_new(const char *, const char *);
/*calloc with allocation check.
first arg - blocks to allocate.
second arg - block size.*/
void *ccalloc(unsigned int, unsigned int);
#endif