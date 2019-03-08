#ifndef ERROR_HANDLE_H
#define ERROR_HANDLE_H
/*the following function returns the program's error status*/
int error();
/*reset error flag*/
void reset_error();
/*the following function receives an error indicator as int and a line specification.
the error ind is the return value from get_CSV_arg. if the indicator is positive, the appropriate msg is displayed.*/
int error_hndl(int);
/*this function takes a string as arguments and compares it to a list of values (strings) in cmd_string.
the index of every such string is corresponding to it's ID as defined in the enum command list in database header.
if the cmd exists, its id is returned, otherwise -1.*/
int cmd_identify(char *cmd);
/*input - cmd string, output is the id of the cmd if supported. UDEF_CMD otherwise. 
if the string start with a dot, the line may be an instruction or a declaration. in this case POSSIBLE_INS is returned.*/
int is_cmd(char *cmd);
/*this function takes a string as arguments and compares it to a list of values (strings) in ins_string.
the index of every such string is corresponding to its ID as defined in the enum ins list in database header.
returns the id or -1 if not supported.*/
int ins_identify(char *ins);
/*check if label contains illegal characters or exceeded allowed length, argument is a string.
return TRUE if label is valid, otherwise the error code is returned.*/
int label_check(char *label);
/*input - data_t node, the function displays warnings in case of: node has label only or extern/entry as a label.*/
int ignore_label(data_t node);
/*the following function classify the given line to its type.
input the first word (non label) of a line.
returns:
line type, or the proper error code.*/
int identify_line_type(char *cmd);
/*the following function checks if the given string is a register. if so, the id of the register is returned.
otherwise -1 is returned.*/
int is_register(char *);
/*the following function checks if @"s unary operand is a valid register. input is the command string,
output is FALSE if usage is invalid. otherwise TRUE.*/
int check_register(char *str);
/*the following function checks if the arguments of the given cmd are valid.
if so, the number of machine words required = addresses needed for storage is returned.
in case of failure, error flag is returned.*/
int operand_check(command, data_t);
/*the following function checks if the given .string is valid. if so, the surrounding brackets are removed.
function input is a string.
output 0 - string invalid.
       1 - string is valid.  */
int check_string(char **);
/*the following function checks if the given line contains a redundant label, 
i.e label combined with extern or an entry, or label only.
the line or the label is ignored and freed with accordance to the data given.
input   - data_t address
        - string address containing the extracted line
        - address of the line counter
output  - FALSE if line was dropped
        - FRUE if the lines is to be stored.
*/
int check_ln_label(data_t **, char **);
/*check if the given string is a reserved machine word, returns TRUE 1 or FALSE 0*/
int is_reserved_word(char *);
/*the following function checks if the given line string is a comment.
if so, the string is freed from memory and the line counter is increased.
input   - the address of the line string
        - the address of the line counter
output  - TRUE if the given line is a comment
        - FALSE otherwise.*/
int is_comment(char **, int *);
#endif