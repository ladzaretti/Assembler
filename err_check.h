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
/*the following function classifys the given line to its type.
input the first word (non label) of a line.
returns:
0 - invalid line
1 - command line
2 - insturction line
*/
int identify_line_type(char *cmd);
/*the following function checks if the given string is a register. if so, the id of the register is returned.
otherwise -1 is returned.*/
int is_register(char *);
/*the following function checks if the arguments of the given cmd are valid.
if so, the number of "words" = addresses needed for storage is returned.
in case of failure, error flag is updated to TRUE.*/
int cmd_operand_check(command, data_t);
int check_string(char **);
#endif