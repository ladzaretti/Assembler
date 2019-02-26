#ifndef PARSER_H
#define PARSER_H
/*the following function receives a path as string and extracts its filename. the function then returns the filename as a string.*/
char *path_fname_extract(char *);
/*the following function gets a string of a line as an argument, returns label if exists, otherwise NULL*/
char *get_label(char **);
/*the following function removes whitespaces from the given data.*/
void remove_wspaces(char **);
/*the following function removes leading whitespaces from the given data.*/
void remove_leading_wspaces(char **str);
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
char *get_nxt_word(char **);
/*the following function gets a given string (line from given file), and stores its componenets in the 
coresponding field in the data structure object.
the first word is checked, if the last char of the word is ':" then the word stored as a label,otherwise as a cmd.
the rest of the line, if there is any is stored as the cmd arguments.*/
data_t *get_data(char **);
/*the following function receives a string representing an integer number.
using strtod, the string is converted to a double. if the double has a decimal value other then zero, the convertion fails.
zero is returned. the vessel num doesnt changed.
if successful, 1 is return and num gets the whole part of the double.*/
int get_num(char *, int *);
#endif