#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include "database.h"
typedef enum
{
    ILLEGAL_COMMA = SHRT_MIN, /*illegal comma*/
    CON_COMMA,                /*multiple consecutive commas*/
    EXT_TEXT,                 /*extraneous text after end of command*/
    ALC_FAILED,               /*Allocation failed*/
    MIS_COMMA,                /*missing comma*/
    UDEF_CMD,                 /*undefined command*/
    UDEF_INS,                 /*undefined data*/
    FIRST_CHR_NON_ALPHA,      /*label first char isnt alphabetic*/
    LABEL_RES_WORD,           /*reserved word is used as a label*/
    LBL_LONG,                 /*label exceeds max len*/
    LBL_ILLEGAL_CHAR,         /*label contains illegal char.*/
    INVALID_REG_USE,          /*missing unary operator - @ on reg*/
    INVALID_UNARY_OP,         /*invalid @ unary operator's operand*/
    STR_UNPRINTABLE_CHR,      /*.string contains unprintable char*/
    STR_MISSING_BRACKET,      /*.string type declatation missing bracket*/
    TOO_FEW_OPERANDS,         /*too many operands for command*/
    TOO_MANY_OPERANDS,        /*missing  operands for command*/
    UNS_REG_SRC,              /*register as a source is unsupported*/
    UNS_SRC_HASHING           /*unsupported source hashing method*/
} error_list;
/*the following function receives an error indicator as int and a line specification.
the error ind is the return value from get_CSV_arg. if the indicator is possitive, the appropriate msg is displayed.*/
int error_hndl(error_list err_num)
{
    if (err_num < 0) /*handle errors from get_CSV_arg. all of them are comma specific.*/
    {
        err = TRUE;
        switch (err_num)
        {
        case ILLEGAL_COMMA:
            printf("error: illegal comma [%s.AS ln %d]\n", file_name, ln_cnt);
            break;
        case CON_COMMA:
            printf("error: multiple consecutive commas [%s.AS ln %d]\n", file_name, ln_cnt);
            break;
        case EXT_TEXT:
            printf("error: extraneous text after end of command [%s.AS ln %d]\n", file_name, ln_cnt);
            break;
        case ALC_FAILED:
            printf("Allocation failed.\n");
            break;
        case MIS_COMMA:
            printf("error: missing comma [%s.AS ln %d]\n", file_name, ln_cnt);
            break;
        case UDEF_CMD:
            printf("error: undefined command name [%s.AS ln %d]\n", file_name, ln_cnt);
            break;
        case UDEF_INS:
            printf("error: undefined data type [%s.AS ln %d]\n", file_name, ln_cnt);
            break;
        case FIRST_CHR_NON_ALPHA:
            printf("error: first character is not alphabetic [%s.AS ln %d]\n", file_name, ln_cnt);
            break;
        case LABEL_RES_WORD:
            printf("error: reserved word as a label [%s.AS ln %d]\n", file_name, ln_cnt);
            break;
        case LBL_LONG:
            printf("error: labal execceds maximum length [%s.AS ln %d]\n", file_name, ln_cnt);
            break;
        case LBL_ILLEGAL_CHAR:
            printf("error: labal contains illigal characters [%s.AS ln %d]\n", file_name, ln_cnt);
            break;
        case INVALID_REG_USE:
            printf("error: invalid register usage, missing unary operator - @ [%s.AS ln %d]\n", file_name, ln_cnt);
            break;
        case INVALID_UNARY_OP:
            printf("error: invalid @ unary operator's operand, expecting register [%s.AS ln %d]\n", file_name, ln_cnt);
            break;
        case STR_UNPRINTABLE_CHR:
            printf("error: string contains unprintable characters [%s.AS ln %d]\n", file_name, ln_cnt); /*check for unprintable chars*/
            break;
        case STR_MISSING_BRACKET:
            printf("error: missing brackets in string [%s.AS ln %d]\n", file_name, ln_cnt);
            break;
        case TOO_FEW_OPERANDS:
            printf("error: too few operands for [%s.AS ln %d]\n", file_name, ln_cnt);
            break;
        case TOO_MANY_OPERANDS:
            printf("error: too many operands for [%s.AS ln %d]\n", file_name, ln_cnt);
            break;
        case UNS_REG_SRC:
            printf("error: lea - register as a source is unsupported [%s.AS ln %d]\n", file_name, ln_cnt);
            break;
        case UNS_SRC_HASHING:
            printf("error: unsupported source hashing method [%s.AS ln %d]\n", file_name, ln_cnt);
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
        error_hndl(UDEF_CMD);
    return UDEF_CMD; /* isnt cmd, or not supported cmd.*/
}
/*this function takes a string as arguments and compares it to a list of values (strings) in ins_string.
the index of every such string is according to its ID as defined in the enum ins list in database header.
returns the id or -1 if not supported.*/
int ins_identify(char *ins)
{
    int i = 0;                                                          /*counter used in the for loop.*/
    symbol_type ins_enum_id;                                            /*variable of type instruction, each and every entry in the given enum is coordinated with the corresponding string in ins_string.*/
    for (ins_enum_id = DATA; ins_enum_id <= EXTERN; ins_enum_id++, i++) /*loop thought enum list.*/
    {
        if (!strcmp(ins, ins_string[i])) /*compare given string to each and every ins_string entry*/
            return ins_enum_id;          /*if ins is supported, its enum ID is returned for future reference.*/
    }
    if (ins[0] == '.') /*if strcmp didnt succeed and first char is a dot.the supposed ins is invalid.*/
        error_hndl(UDEF_INS);
    return UDEF_INS; /*not a ins, or not supported ins.*/
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
    if ((ins_identify(cmd) == UDEF_INS) && (is_cmd(cmd) >= 0))
        return 1; /*the line is a command.*/
    if ((ins_identify(cmd) >= 0) && (is_cmd(cmd) == UDEF_CMD))
        return 2; /*the line is an instruction.*/
    return 0;     /*invalid line.*/
}
/*check if label contains illigal characters of execced allowed lenght, argument is a string.*/
void label_check(char *label)
{
    int i = 0;
    if (!(isalpha(label[0])))
        error_hndl(FIRST_CHR_NON_ALPHA);
    if (cmd_identify(label) >= 0)
        error_hndl(LABEL_RES_WORD);
    if (strlen(label) > LABEL_MAX_LEN)
        error_hndl(LBL_LONG);
    for (; i < strlen(label); i++)
        if ((!isalpha(label[i])) && (!isdigit(label[i])))
        {
            error_hndl(LBL_ILLEGAL_CHAR);
            return;
        }
}
/*input - data_t node, the fucntion displays warnings in case of: node has label only or extern/enrty as a label.*/
int ignore_label(data_t node)
{
    if ((node.label) && (!node.cmd) && (!node.arg)) /*if the given line has label only, warning is displayed.*/
    {
        printf("warning: line contains label only - ignored [%s.AS ln %d]\n", file_name, ln_cnt);
        return 1; /*line to be ignored.*/
    }
    if ((node.label) && (((strcmp(node.cmd, ".extern") == 0)) || ((strcmp(node.cmd, ".entry")) == 0))) /*if the line is an entry/extern and has a label, warning is displayed.*/
    {
        printf("warning: label with extern/entry is not supported [%s.AS ln %d]\n", file_name, ln_cnt);
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
    if ((str[0] == '@') && ((register_id(str + 1)) >= 0)) /*@'s unary operand is a register*/
        return TRUE;
    if ((str[0] == '@') && ((register_id(str + 1)) == -1)) /*@"s operand isnt a register*/
    {
        error_hndl(INVALID_UNARY_OP);
        return FALSE;
    }
    if ((str[0] != '@') && ((register_id(str)) >= 0)) /* use of a register with out @*/
    {
        error_hndl(INVALID_REG_USE);
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
        error_hndl(TOO_MANY_OPERANDS);
        return 0;
    }
    if ((node.narg < cmd_num_op[id]) && (node.narg >= 0))
    {
        error_hndl(TOO_FEW_OPERANDS);
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
        if ((is_register(*arg)) && (is_register(*(arg + 1)))) /*check if both of the arguments are registers.*/
        {
            return 1;
        } /*return 1 as the needed space in memory, two registers in one machine "word".*/
        break;
        /*src hash method = 3. dec hash method = 3,5*/
    case (LEA):
        if ((**arg == '@') && (register_id(*arg + 1)))
        {
            error_hndl(UNS_SRC_HASHING);
        }
        if ((register_id(*arg)) >= 0)
        {
            error_hndl(UNS_REG_SRC);
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
/*the following function checks if the given line contains a redundent lable, 
i.e label combined with extern or an entry, or label only.
the line or the label is ignored and freed with accordance to the data given.
input   - data_t address
        - string address containing the extracted line
        - address of the line counter
output  - FALSE if line was dropped
        - FRUE if the lines is to be stored.
*/
int check_line_label(data_t **pdata, char **line_st, int *l_cnt)
{
    int ign_label = ignore_label(**pdata); /*get label status. 1 = line has label only, 2 = ext/ent with label, 0 = label is valid.*/
    if (ign_label == 1)                    /*free current node, as its being ignored. line has label only.*/
    {
        free((*pdata)->label); /*free label string*/
        free(*line_st);        /*free current line string.*/
        free(*pdata);          /*free node*/
        (*l_cnt)++;
        return FALSE; /*return false as the line is being ignored.*/
    }
    if (ign_label == 2) /*line contains entry/extern and a label.*/
    {
        free((*pdata)->label);  /*ignore label.*/
        (*pdata)->label = NULL; /*set label ptr to null.*/
    }
    return TRUE; /*line is valid*/
}