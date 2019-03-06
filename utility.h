#ifndef UTILITY_H
#define UTILITY_H
#include <stdio.h>
/*the following function receives a string representing an integer number.
using strtod, the string is converted to a double. if the double has a decimal value other then zero, the convertion fails.
zero is returned. the vessel num doesnt changed.
if successful, 1 is return and num gets the whole part of the double.*/
int get_num(char *, int *);
/*the following function receives a path as string and extracts its filename. the function then returns the filename as a string.*/
char *path_fname_extract(const char *);
/*concatenate the given strings into a new string without changing the input.
input - first argument is the beginning, the second string is the ending.
return pointer to a new allocated string containing the result.*/
char *strcat_new(const char *, const char *);
/*calloc with allocation check.
first arg - blocks to allocate.
second arg - block size.*/
void *ccalloc(unsigned int, unsigned int);
/*use argv to a open file, argv can contain an extension or not.
file_name will be asigned with the extensionless file name.
also, extension is non case sensetive*/
FILE *dyn_fopen(const char *, char **);
#endif