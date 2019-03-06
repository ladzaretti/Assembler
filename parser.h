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
coresponding field in the data structure object data_t.
the function assumes that whitespaces between the label and the colon, and a dot and it'ss associated data type are allowed.
the rest of the line is  considere considered as the command's arguments*/
data_t *get_data(char **);
#endif