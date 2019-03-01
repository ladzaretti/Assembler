#ifndef PARSER_H
#define PARSER_H
/*get input dynamically from stdio stream(keyboard/redirection).
function arguments: pointer to an array of char (string)
return values:
1 - finished successfully
0 - allocation failed
EOF = -1 - reached input's EOF*/
int fget_line(char **, FILE *);
/*the following function gets a given string (line from given file), and stores its componenets in the 
coresponding field in the data structure object.
the first word is checked, if the last char of the word is ':" then the word stored as a label,otherwise as a cmd.
the rest of the line, if there is any is stored as the cmd arguments.*/
data_t *get_data(char **);
#endif