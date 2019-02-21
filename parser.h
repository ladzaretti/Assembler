#ifndef PARSER_H
#define PARSER_H
#include "database.h"
/*the following function receives a path as string and extracts its filename. the function then returns the filename as a string.*/
char *path_fname_extract(char *);
/*the following function gets a string of a line as an argument, returns label if exists, otherwise NULL*/
char *get_label(char **);
/*the following function removes whitespaces from the given data.*/
void remove_wspaces(char **);
/*get input dynamically from stdio stream(keyboard/redirection).
function arguments: pointer to an array of char (string)
return values:
1 - finished successfully
0 - allocation failed
EOF = -1 - reached input's EOF*/
int fget_line(char **, FILE *);
/*the following function receives a pointer to an array of char (string) as arguments.
the function returns pointer to the extracted first word in the given pointer to string.
if the given data contains no data, an empty string will be returned ("/n")*/
char *get_cmd(char **);
void get_data(char **, data_t **);
#endif