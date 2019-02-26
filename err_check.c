#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "database.h"
/*the following function receives an error indicator as int and a line specification.
the error ind is the return value from get_CSV_arg. if the indicator is possitive, the appropriate msg is displayed.*/
int err_invalid_cms(int err_num)
{
    if (err_num < 0) /*handle errors from get_CSV_arg. all of them are comma specific.*/
    {
        switch (err_num)
        {
        case -1:
            printf("error: illegal comma [line %d]\n", linec);
            err = TRUE;
        case -2:
            printf("error: multiple consecutive commas [line %d]\n", linec);
            err = TRUE;
        case -3:
            printf("error: extraneous text after end of command [line %d]\n", linec);
            err = TRUE;
        case -4:
            printf("Allocation failed.\n");
            err = TRUE;
        case -5:
            printf("error: missing comma [line %d]\n", linec);
            err = TRUE;
        }
    }
    return err_num;
}
/*this function takes a string as arguments and compares it to a list of values (strings) in cmd_string.
the index of every such string is according to its ID as defined in the enum command list in database header.
if the cmd exists, its id is returned, otherwise -1.*/
int cmd_identify(char *cmd)
{
    int i = 0;                                                       /*counter used in the for loop.*/
    command cmd_enum_id;                                             /*variable of type command, each and every entry in the given enum is coordinated with the corresponding string in cmd_string.*/
    for (cmd_enum_id = MOV; cmd_enum_id <= STOP; cmd_enum_id++, i++) /*loop thought enum list.*/
    {
        if (!strcmp(cmd, cmd_string[i])) /*compare given string to each and every cmd_string entry*/
            return cmd_enum_id;          /*if cmd is one of the supported commands, its enum ID is returned for future reference.*/
    }
    return -1;
}
/*input - cmd strimg, output is the id of the cmd if supported. or -1 otherwise. 
if the string doesnt start with a dot, the line has an error.*/
int is_cmd(char *cmd)
{
    int id;
    id = cmd_identify(cmd);
    if (id >= 0)
        return id;     /*return id.*/
    if (cmd[0] != '.') /*if strcmp didnt succeed and first char is not a dot. the supposed cmd is invalid.*/
    {
        printf("error: <%s> - undefined command name [line %d]\n", cmd, linec);
        err = 1;
    }
    return -1; /* isnt cmd, or not supported cmd.*/
}
/*this function takes a string as arguments and compares it to a list of values (strings) in ins_string.
the index of every such string is according to its ID as defined in the enum ins list in database header.
returns the id or -1 if not supported.*/
int ins_identify(char *ins)
{
    int i = 0;                                                          /*counter used in the for loop.*/
    instruction ins_enum_id;                                            /*variable of type instruction, each and every entry in the given enum is coordinated with the corresponding string in ins_string.*/
    for (ins_enum_id = DATA; ins_enum_id <= EXTERN; ins_enum_id++, i++) /*loop thought enum list.*/
    {
        if (!strcmp(ins, ins_string[i])) /*compare given string to each and every ins_string entry*/
            return ins_enum_id;          /*if ins is supported, its enum ID is returned for future reference.*/
    }
    if (ins[0] == '.') /*if strcmp didnt succeed and first char is a dot.the supposed ins is invalid.*/
    {
        printf("error: <%s> - undefined data type [line %d]\n", ins, linec);
        err = 1;
    }
    return -1; /*not a ins, or not supported ins.*/
}
/*the following function classifys the given line to its type.
input the first word (non label) of a line.
returns:
0 - invalid line
1 - command line
2 - insturction line
*/
int identify_line_type(char *cmd)
{
    if ((ins_identify(cmd) == -1) && (is_cmd(cmd) >= 0))
        return 1; /*the line is a command.*/
    if ((ins_identify(cmd) >= 0) && (is_cmd(cmd) == -1))
        return 2; /*the line is an instruction.*/
    return 0;     /*invalid line.*/
}
/*check if label contains illigal characters of execced allowed lenght, argument is a string.*/
void label_check(char *label)
{
    int i = 0;
    if (!(isalpha(label[0])))
    {
        err = 1;
        printf("error: <%s> - first character is not alphabetic [line %d]\n", label, linec);
        return;
    }
    if (cmd_identify(label) >= 0)
    {
        err = 1;
        printf("error: <%s> - reserved word as a label [line %d]\n", label, linec);
        return;
    }
    if (strlen(label) > LABEL_MAX_LEN)
    {
        err = 1;
        printf("error: labal execceds maximum length [line %d]\n", linec);
        return;
    }
    for (; i < strlen(label); i++)
        if ((!isalpha(label[i])) && (!isdigit(label[i])))
        {
            err = 1;
            printf("error: <%s> - labal contains illigal characters [line %d]\n", label, linec);
            return;
        }
}
/*input - data_t node, the fucntion displays warnings in case of: node has label only or extern/enrty as a label.*/
int ignore_label(data_t node)
{
    if ((node.label) && (!node.cmd) && (!node.arg)) /*if the given line has label only, warning is displayed.*/
    {
        printf("warning: line contains label only - ignored [line %d]\n", linec);
        return 1; /*line to be ignored.*/
    }
    if ((node.label) && (((strcmp(node.cmd, ".extern") == 0)) || ((strcmp(node.cmd, ".entry")) == 0))) /*if the line is an entry/extern and has a label, warning is displayed.*/
    {
        printf("warning: label with extern/entry is not supported [line %d]\n", linec);
        return 2; /*label to be ignored.*/
    }
    return 0; /*label is accepted.*/
}
/*the following function checks if the given string is a register. if so, the id of the register is returned.
otherwise -1 is returned.*/
int register_id(char *str)
{
    int i = 0;
    for (; i <= REG_NUM; i++)
    {
        if ((strcmp(str, registers[i])) == 0)
            return i;
    }
    return -1;
}
/*the following function checks if @"s unnary operand is a valid register. input is the command string,
output is FALSE if usage is invalid. otherwise TRUE.*/
int is_register(char *str)
{
    puts(str);
    if ((str[0] == '@') && ((register_id(str + 1)) >= 0)) /*@'s unary operand is a register*/
        return TRUE;
    if ((str[0] == '@') && ((register_id(str + 1)) == -1)) /*@"s operand isnt a register*/
    {
        printf("error: invalid @ unary operator's operand, expecting register <%s> [line %d]\n", str + 1, linec);
        err = TRUE;
        return FALSE;
    }
    if ((str[0] != '@') && ((register_id(str)) >= 0)) /* use of a register with out @*/
    {
        printf("error: invalid register usage, missing unary operator - @ <%s> [line %d]\n", str, linec);
        err = TRUE;
        return FALSE;
    }
    return FALSE; /*otherwise, no @ operator and not a register.*/
}
/*the following function checks if the arguments of the given cmd are valid.
if so, the number of "words" = addresses needed for storage is returned.
in case of failure, error flag is updated to TRUE.*/
int cmd_operand_check(command id, data_t node)
{
    char **arg = node.arg;
    int cmd_num_op[16] = {2, 2, 2, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 0, 0};
    if (node.narg > cmd_num_op[id])
    {
        printf("error: too many operands for <%s> [line %d]\n", node.cmd, linec);
        err = TRUE;
        return 0;
    }
    if ((node.narg < cmd_num_op[id]) && (node.narg >= 0))
    {
        printf("error: too few operands for <%s> [line %d]\n", node.cmd, linec);
        err = TRUE;
        return 0;
    }
    switch (id)
    {
        /*src hash method = 1,3,5. dec hash method = 1,3,5*/
    case (CMP):
        if ((is_register(*arg)) && (is_register(*(arg + 1)))) /*check if both of the arguments are registers.*/
            return 1;                                         /*return 1 as the needed space in memory, two registers in one machine "word".*/
        break;
        /*src hash method = 1,3,5. dec hash method = 3,5*/
    case (MOV):
    case (ADD):
    case (SUB):
        printf("<%s----%s>\n", *arg, *(arg + 1));
        if ((is_register(*arg)) && (is_register(*(arg + 1)))) /*check if both of the arguments are registers.*/
        {
            puts("blat");
            return 1;
        } /*return 1 as the needed space in memory, two registers in one machine "word".*/
        break;
        /*src hash method = 3. dec hash method = 3,5*/
    case (LEA):
        if ((**arg == '@') && (register_id(*arg + 1)))
        {
            printf("error: unsupported source hashing method <%s> [line %d]\n", *arg, linec);
            err = TRUE;
        }
        if ((register_id(*arg)) >= 0)
        {
            printf("error: lea - register as a source is unsupported <%s> [line %d]\n", *arg, linec);
            err = TRUE;
        }
        is_register(*(arg + 1));
        break;
        /*src hash method = NONE. dec hash method = 3,5*/
    case (NOT):
    case (CLR):
    case (INC):
    case (DEC):
    case (JMP):
    case (BNE):
    case (RED):
    case (JSR):
        is_register(*arg);
        break;
    /*src hash method = NONE. dec hash method = 1,3,5*/
    case (PRN):
        is_register(*arg);
        break;
        /*no operands.*/
    case (RTS):
    case (STOP):
        /* code */
        break;
    default:
        break;
    }
    return cmd_num_op[id];
}
/*the following function checks if the given .string is valid. if so, the sorrounding brackets are removed.
function input is an adress of pointer to string.
output 0 - string invalid.
       1 - string is valid.  */
int check_string(char **str)
{
    int i;
    char *p = (char *)malloc(strlen(*str));
    if (((*(*str) != '\"') || (*(*str + (strlen(*str)) - 1) != '\"')) || ((strlen(*str) == 1) && ((*(*str) == '\"')))) /*check for missing brackets or if the given string is of length 0 and is a bracket*/
    {
        printf("error: missing brackets in string <%s> [line %d]\n", *str, linec);
        err = TRUE;
        free(p);
        return 0;
    }
    for (i = 0; i < strlen(*str); i++)
        if (!(isprint(*(*str + i))))
        {
            printf("error: string contains unprintable characters <%s> [line %d]\n", *str, linec); /*check for unprintable chars*/
            err = TRUE;
            free(p);
            return 0;
        }
    strcpy(p, *str + 1);             /*copy string without the opening bracket.*/
    strncpy(*str, p, strlen(p) - 1); /*overwrite opening bracket.*/
    *(*str + strlen(p) - 1) = 0;     /*terminate string at closing bracket.*/
    free(p);                         /*free temp string pointer.*/
    return 1;
}