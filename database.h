#ifndef DATABASE_H
#define DATABASE_H
#include <limits.h>
#define CMD_LINE 1 /*define command line ID*/
#define INS_LINE 2 /*define instruction line ID.*/
#define TRUE 1
#define FALSE 0
#define REG_NUM 7
#define LABEL_MAX_LEN 31
extern const char cmd_string[16][5];
extern const char ins_string[4][8];
extern const char registers[8][4];
extern char *file_name;
extern int ln_cnt; /*read line counter.*/
extern int err;    /*global error flag*/
extern int IC;
extern int DC;
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
    UNS_SRC_HASHING,          /*unsupported source hashing method*/
    LABEL_EXISTS,             /*label already exists*/
    UNINITILIZED_DATA,        /*uninitilaied .data variable*/
    UNINITILIZED_STRING,      /*uninitilaied .string variable*/
    NON_INT                 /*given variable is not an integer*/

} error_list;
/*enum contaning all the call names for the known commands. ordered is consistent to the array of cmd names (cmd_string) defined in err_check.c.
each record is define explicidly to avoid jumps in value.*/
typedef enum
{
    MOV = 0,
    CMP = 1,
    ADD = 2,
    SUB = 3,
    NOT = 4,
    CLR = 5,
    LEA = 6,
    INC = 7,
    DEC = 8,
    JMP = 9,
    BNE = 10,
    RED = 11,
    PRN = 12,
    JSR = 13,
    RTS = 14,
    STOP = 15
} command;
/*list of known instructions.*/
typedef enum
{
    DATA = 0,
    STRING = 1,
    ENTRY = 2,
    EXTERN = 3,
    COM = 4
} symbol_type;
/*list of differant linked list data types.*/
enum node_type
{
    DATA_T = 0,
    SYMBOL_T = 1
};
/*struct to store parsed line*/
typedef struct data
{
    char *label;
    char *cmd;
    char **arg;
    int narg;
} data_t;
typedef struct symbol
{
    char *label;
    unsigned int address;
    unsigned int command;
    unsigned int external;
} symbol_t;
/*generic double linked list, data fields are changable.*/
typedef struct node *ptr;
typedef struct node
{
    void *data;
    ptr next;
    ptr prev;
} node_t;
/*generic list struct, contains head and tail pointers.*/
typedef struct list
{
    ptr head;
    ptr tail;
} list_t;
#endif