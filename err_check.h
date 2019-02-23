#ifndef ERR_CHECK_H
#define ERR_CHECK_H
/*the following function receives an error indicator as int and a line specification.
the error ind is the return value from get_CSV_arg. if the indicator is possitive, the appropriate msg is displayed.*/
int err_invalid_cms(int);
/*this function takes a string as arguments and compares it to a list of values (strings) in cmd_string.
the index of every such string is according to its ID as defined in the enum command list in complex header.
if exists, the id of the cmd is return, otherwise -1.*/
int cmd_identify(char *cmd);
/*input - cmd strimg, output is the id of the cmd if supported. or -1 otherwise. 
if the string doesnt start with a dot, the line has an error.*/
int is_cmd(char *cmd);
/*this function takes a string as arguments and compares it to a list of values (strings) in ins_string.
the index of every such string is according to its ID as defined in the enum ins list in database header.
if exists, the id of the ins is return, otherwise -1.*/
int ins_identify(char *ins);
/*the following function check wether the given string contains chars other than letters/digits.*/
void label_check(char *label);
/*input - data_t node, the fucntion displays warnings in case of: node has label only or extern/enrty as a label.*/
int ignore_label(data_t node);
#endif