#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "database.h"
#define IGNORE_LABEL 1
#define ACCEPT_LABEL 2
#define IGNORE_LINE 3
static int err = FALSE; /*error flag*/
/*macro for printing error messages to stdout, input is a constant string.
outputs to stdout the given string with the current line and file.*/
#define print_error(x)                                  \
    do                                                  \
    {                                                   \
        printf("error: ");                              \
        fputs(x, stdout);                               \
        printf(" [%s.AS, ln %d]\n", file_name, ln_cnt); \
    } while (0)
/*the following function returns the program's error status*/
int error()
{
    return err;
}
/*reset error flag*/
void reset_error()
{
    err = FALSE;
}
/*the following function receives an error indicator as int and a line specification.
the error ind is the return value from get_CSV_arg. if the indicator is possitive, the appropriate msg is displayed.*/
int error_hndl(error_list err_num)
{
    if ((err_num >= 0))
        return err_num;
    else /*handle errors*/
    {
        err = TRUE;
        switch (err_num)
        {
        case ILLEGAL_COMMA:
            print_error("illegal_comma");
            break;
        case CON_COMMA:
            print_error("multiple consecutive commas");
            break;
        case EXT_TEXT:
            print_error("extraneous text after end of command");
            break;
        case ALC_FAILED:
            printf("Allocation failed, line %d, file %s.\n", __LINE__, __FILE__);
            break;
        case MIS_COMMA:
            print_error("missing comma");
            break;
        case UDEF_CMD:
            print_error("undefined command name");
            break;
        case UDEF_INS:
            print_error("undefined data type");
            break;
        case FIRST_CHR_NON_ALPHA:
            print_error("first character is not alphabetic");
            break;
        case LABEL_RES_WORD:
            print_error("reserved word as a label");
            break;
        case LBL_LONG:
            print_error("labal execceds maximum length");
            break;
        case LBL_ILLEGAL_CHAR:
            print_error("labal contains illegal characters");
            break;
        case INVALID_REG_USE:
            print_error("invalid register usage, missing unary operator");
            break;
        case INVALID_ARGUMENT:
            print_error("invalid argument");
            break;
        case INVALID_UNARY_OP:
            print_error("invalid @ unary operator's operand, expecting register");
            break;
        case STR_UNPRINTABLE_CHR:
            print_error("string contains unprintable characters");
            break;
        case STR_MISSING_BRACKET:
            print_error("missing brackets in string");
            break;
        case TOO_FEW_OPERANDS:
            print_error("missing operands for given command");
            break;
        case TOO_MANY_OPERANDS:
            print_error("too many operands for given command");
            break;
        case UNS_REG_SRC:
            print_error("lea - register as a source is unsupported");
            break;
        case UNS_SRC_HASHING:
            print_error("unsupported source hashing method");
            break;
        case LABEL_EXISTS:
            print_error("label already exists");
            break;
        case UNINITILIZED_DATA:
            print_error("uninitialized .data variable");
            break;
        case UNINITILIZED_STRING:
            print_error("uninitialized .string variable");
            break;
        case NON_INT:
            print_error("argument is not an integer");
            break;
        case CMD_AS_VAR:
            print_error("function cannot operate on a command");
            break;
        case VAR_AS_CMD:
            print_error("function cannot operate on a variable");
            break;
        case UDEF_VAR:
            print_error("undefined variable");
            break;
        case UDEF_REF:
            print_error("undefined reference");
            break;
        case EXT_AND_ENTRY:
            print_error("variable is both external and an entry");
            break;
        case UNDECLARED_ENTRY:
            print_error("undeclared entry");
            break;
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
    int cmd_enum_id;                                                 /*variable of type command, each and every entry in the given enum is coordinated with the corresponding string in cmd_string.*/
    for (cmd_enum_id = MOV; cmd_enum_id <= STOP; cmd_enum_id++, i++) /*loop thought enum list.*/
    {
        if (!strcmp(cmd, cmd_string[i])) /*compare given string to each and every cmd_string entry*/
            return cmd_enum_id;          /*if cmd is one of the supported commands, its enum ID is returned for future reference.*/
    }
    return -1;
}
/*input - cmd strimg, output is the id of the cmd if supported. or -1 otherwise. 
if the string start with a dot, the line may be an instruction or declaration.*/
int is_cmd(char *cmd)
{
    int id;
    id = cmd_identify(cmd);
    if (id >= 0)
        return id;     /*return id.*/
    if (cmd[0] != '.') /*if strcmp didnt succeed and first char is not a dot. the supposed cmd is invalid.*/
        return UDEF_CMD;
    return POSSIBLE_INS; /* isnt cmd, possibly an ins*/
}
/*input - instruction strimg, output is the id of the ins if supported. or -1 otherwise. */
int is_instruction(char *str)
{
    int i = DATA;
    for (; i <= EXTERN; i++) /*loop thought enum list.*/
    {
        if ((strcmp(str, ins_string[i])) == 0) /*compare given string to each and every ins_string entry*/
            return i;                          /*if ins is supported, its enum ID is returned for future reference.*/
    }
    return -1;
}
/*this function takes a string as arguments and compares it to a list of values (strings) in ins_string.
the index of every such string is according to its ID as defined in the enum ins list in database header.
returns the id or -1 if not supported.*/
int ins_identify(char *ins)
{
    int ins_enum_id;   /*variable of type instruction, each and every entry in the given enum is coordinated with the corresponding string in ins_string.*/
    if (ins[0] == '.') /*if strcmp didnt succeed and first char is a dot.the supposed ins is invalid.*/
    {
        if ((strlen(ins) > 1) && ((ins_enum_id = is_instruction(ins + 1)) >= 0)) /*if the string isnt ".", check if the text after the dot is a type of data*/
            return ins_enum_id;                                                  /*if so, return data id.*/
        else
            return UDEF_INS; /*not supported ins*/
    }
    return POSSIBLE_CMD; /*not a ins, possibly a cmd*/
}
/*the following function classifys the given line to its type.
input the first word (non label) of a line.
returns:
line type, or the proper error code.*/
int identify_line_type(char *cmd)
{
    int cmd_id = is_cmd(cmd);
    int ins_id = ins_identify(cmd);
    if (ins_id == UDEF_INS)
        return UDEF_INS;
    if (cmd_id == UDEF_CMD)
        return UDEF_CMD;
    if (cmd_id >= 0)
        return CMD_LINE; /*the line is a command.*/
    return INS_LINE;     /*the line must be an instruction.*/
}
/*the following function checks if the given string is a register. if so, the id of the register is returned.
otherwise -1 is returned.*/
int is_register(char *str)
{
    int i = 0;
    for (; i < REG_NUM; i++)
    {
        if ((strcmp(str, registers[i])) == 0)
            return i;
    }
    return -1;
}
/*the following function checks if @"s unnary operand is a valid register. input is the command string,
output is FALSE if usage is invalid. otherwise TRUE.*/
int check_register(char *str)
{
    if ((str[0] == '@') && ((is_register(str + 1)) >= 0)) /*@'s unary operand is a register*/
        return TRUE;
    if ((str[0] == '@') && ((is_register(str + 1)) == -1)) /*@"s operand isnt a register*/
    {
        error_hndl(INVALID_UNARY_OP);
        return INVALID_UNARY_OP;
    }
    if ((str[0] != '@') && ((is_register(str)) >= 0)) /* use of a register with out @*/
    {
        error_hndl(INVALID_REG_USE);
        return INVALID_REG_USE;
    }
    return FALSE; /*otherwise, no @ operator and not a register.*/
}
/*check if the given string is a reserved machine word*/
int is_reserved_word(char *str)
{
    if ((cmd_identify(str) >= 0) || (is_register(str) >= 0) || (is_instruction(str) >= 0))
        return TRUE;
    else
        return FALSE;
}
/*check if label contains illegal characters or execced allowed length, argument is a string.
return TRUE if label is valid, otherwise the error code is returned.*/
int label_check(char *label)
{
    int i = 0;
    if (!(isalpha(label[0])))
        return FIRST_CHR_NON_ALPHA;
    if (is_reserved_word(label))
        return LABEL_RES_WORD;
    if (strlen(label) > LABEL_MAX_LEN)
        return LBL_LONG;
    for (; i < strlen(label); i++)
        if ((!isalpha(label[i])) && (!isdigit(label[i])))
            return LBL_ILLEGAL_CHAR;
    return TRUE;
}
/*input - data_t node, the fucntion displays warnings in case of: node has label only or extern/enrty as a label.*/
int ignore_label(data_t node)
{
    if ((node.label) && (!node.cmd) && (!node.arg)) /*if the given line has label only, warning is displayed.*/
    {
        printf("warning: line contains label only - ignored [%s.AS, ln %d]\n", file_name, ln_cnt);
        return IGNORE_LINE; /*line to be ignored.*/
    }
    if ((node.label) && (((strcmp(node.cmd, ".extern") == 0)) || ((strcmp(node.cmd, ".entry")) == 0))) /*if the line is an entry/extern and has a label, warning is displayed.*/
    {
        printf("warning: label with extern/entry is not supported [%s.AS, ln %d]\n", file_name, ln_cnt);
        return IGNORE_LABEL; /*label to be ignored.*/
    }
    return ACCEPT_LABEL; /*label is accepted.*/
}
/*the following function checks if the given line contains a redundent lable, 
i.e label combined with extern or an entry, or label only.
the line or the label is ignored and freed with accordance to the data given.
input   - data_t address
        - string address containing the extracted line
        - address of the line counter
output  - FALSE if line was dropped
        - FRUE if the lines is to be stored.
*/
int check_ln_label(data_t **pdata, char **line_st)
{
    int ign_label = ignore_label(**pdata); /*get label status. 1 = line has label only, 2 = ext/ent with label, 0 = label is valid.*/
    if (ign_label == IGNORE_LINE)          /*free current node, as its being ignored. line has label only.*/
    {
        free((*pdata)->label); /*free label string*/
        free(*line_st);        /*free current line string.*/
        free(*pdata);          /*free node*/
        return FALSE; /*return false as the line is being ignored.*/
    }
    if (ign_label == IGNORE_LABEL) /*line contains entry/extern and a label.*/
    {
        free((*pdata)->label);  /*ignore label.*/
        (*pdata)->label = NULL; /*set label ptr to null.*/
    }
    return TRUE; /*line is valid*/
}
/*the following function checks if the arguments of the given cmd are valid.
if so, the number of "words" = addresses needed for storage is returned.
in case of failure, error flag is returned.*/
int operand_check(command id, data_t node)
{
    char **arg = node.arg;
    char *ptr; /*dummy for strtod*/
    int cmd_num_op[16] = {2, 2, 2, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 0, 0};
    if (node.narg > cmd_num_op[id])
        return TOO_MANY_OPERANDS;
    if ((node.narg < cmd_num_op[id]) && (node.narg >= 0))
        return TOO_FEW_OPERANDS;
    switch (id)
    {
        /*src hash method = 1,3,5. dec hash method = 1,3,5*/
    case (CMP):
        if ((check_register(*arg)) && (check_register(*(arg + 1)))) /*check if both of the arguments are registers.*/
            return 1;                                               /*return 1 as the needed space in memory, two registers in one machine "word".*/
        break;
        /*src hash method = 1,3,5. dec hash method = 3,5*/
    case (MOV):
    case (ADD):
    case (SUB):
        if (strtod(*(arg + 1), &ptr) != 0.0) /*check for illegal destination i.e immediate  number*/
            error_hndl(INVALID_ARGUMENT);
        if ((check_register(*arg)) && (check_register(*(arg + 1)))) /*check if both of the arguments are registers.*/
        {
            return 1;
        } /*return 1 as the needed space in memory, two registers in one machine "word".*/
        break;
        /*src hash method = 3. dec hash method = 3,5*/
    case (LEA):
        /*check source*/
        if ((**arg == '@') && (is_register(*arg + 1))) /*source is a register*/
            error_hndl(UNS_SRC_HASHING);
        if (strtod(*(arg), &ptr) != 0.0) /*source is a number*/
            error_hndl(INVALID_ARGUMENT);
        if ((is_register(*arg)) >= 0)
        {
            error_hndl(UNS_REG_SRC);
        }
        check_register(*(arg + 1));
        if (strtod(*(arg + 1), &ptr) != 0.0) /*check for illegal destination i.e immediate  number*/
            error_hndl(INVALID_ARGUMENT);
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
        if (strtod(*arg, &ptr) != 0.0) /*ie the argument is some kind of number.*/
            error_hndl(INVALID_ARGUMENT);
        break;
    /*src hash method = NONE. dec hash method = 1,3,5*/
    case (PRN):
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
    if (*(*str) == 0)
    {
        error_hndl(UNINITILIZED_STRING);
        free(p);
        return 0;
    }
    if (((*(*str) != '\"') || (*(*str + (strlen(*str)) - 1) != '\"')) || ((strlen(*str) == 1) && ((*(*str) == '\"')))) /*check for missing brackets or if the given string is of length 0 and is a bracket*/
    {
        error_hndl(STR_MISSING_BRACKET);
        free(p);
        return 0;
    }
    for (i = 0; i < strlen(*str); i++)
        if (!(isprint(*(*str + i))))
        {
            error_hndl(STR_UNPRINTABLE_CHR);
            free(p);
            return 0;
        }
    strcpy(p, *str + 1);             /*copy string without the opening bracket.*/
    strncpy(*str, p, strlen(p) - 1); /*overwrite opening bracket.*/
    *(*str + strlen(p) - 1) = 0;     /*terminate string at closing bracket.*/
    free(p);                         /*free temp string pointer.*/
    return 1;
}
/*the following function checks if the given line string is a comment.
if so, the string is freed from memory and the line counter is increased.
input   - the address of the line string
        - the address of the line counter
output  - TRUE if the given line is a comment
        - FALSE otherwise.*/
int is_comment(char **line, int *ln_cnt)
{
    if (*line[0] == ';') /*check if the given line is a comment.*/
    {
        (*ln_cnt)++;
        free(*line);
        return TRUE;
    }
    return FALSE;
}