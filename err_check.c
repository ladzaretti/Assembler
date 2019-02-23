#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "database.h"
#define LABEL_MAX_LEN 31
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
            return err = 1;
        case -2:
            printf("error: multiple consecutive commas [line %d]\n", linec);
            return err = 1;
        case -3:
            printf("error: extraneous text after end of command [line %d]\n", linec);
            return err = 1;
        case -4:
            printf("Allocation failed.\n");
            return err = 1;
        case -5:
            printf("error: missing comma.\n [line %d]", linec);
            return err = 1;
        }
    }
    return 0;
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