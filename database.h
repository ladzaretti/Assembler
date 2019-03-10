#ifndef DATABASE_H
#define DATABASE_H
#include <limits.h>
#define CMD_LINE 1      /*define command line ID*/
#define POSSIBLE_CMD -1 /*not a ins, possibly a cmd*/
#define INS_LINE 2      /*define instruction line ID.*/
#define POSSIBLE_INS -2 /*not a cmd, possibly ins.*/
#define TRUE 1
#define FALSE 0
#define REG_NUM 8 /*number of registers*/
#define LABEL_MAX_LEN 31
#define STR_ADDRESS 100
#define BITS 10
extern char *file_name;
extern int ln_cnt; /*read line counter.*/
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
    UDEF_VAR,                 /*undefined variable*/
    UDEF_REF,                 /*undefined reference*/
    FIRST_CHR_NON_ALPHA,      /*label first char isnt alphabetic*/
    LABEL_RES_WORD,           /*reserved word is used as a label*/
    LBL_LONG,                 /*label exceeds max len*/
    LBL_ILLEGAL_CHAR,         /*label contains illegal char.*/
    INVALID_REG_USE,          /*missing unary operator - @ on reg*/
    INVALID_UNARY_OP,         /*invalid @ unary operator's operand*/
    INVALID_ARGUMENT,         /*invalid argument*/
    STR_UNPRINTABLE_CHR,      /*.string contains unprintable char*/
    STR_MISSING_BRACKET,      /*.string type declaration missing bracket*/
    MISSING_OPERANDS,         /*too many operands for command*/
    TOO_MANY_OPERANDS,        /*missing  operands for command*/
    UNS_REG_SRC,              /*register as a source is unsupported*/
    UNS_SRC_HASHING,          /*unsupported source hashing method*/
    LABEL_EXISTS,             /*label already exists*/
    UNINITIALIZED_DATA,       /*uninitialized .data variable*/
    UNINITIALIZED_STRING,     /*uninitialized .string variable*/
    NON_INT,                  /*given variable is not an integer*/
    CMD_AS_VAR,               /*using a command as cmd argument*/
    VAR_AS_CMD,               /*using a var as cmd argument*/
    EXT_AND_ENTRY,            /*both external and an entry*/
    UNDECLARED_ENTRY,         /*undeclared entry*/
    INT_TOO_BIG,              /*given int is too big*/
    INT_TOO_SMALL             /*given int is too small*/
} error_list;
/*enum containing all the call names for the known commands. ordered is consistent to the array of cmd names (cmd_string) defined in error.c.
each record is define explicitly to avoid jumps in value.*/
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
    COM = 4 /*extra id for a line contains a command*/
} symbol_type;
/*list of different linked list data types.*/
enum node_type
{
    DATA_T = 0,
    SYMBOL_T = 1,
    INT_BIN_T = 2,
    EXTERNAL_T = 3,
    ENTRT_P = 4, /*for printing purposes. in actuality is of type symbol_t*/
    BASE64_P = 5 /*not an actual node, for printing purposes only*/
};
/*struct to store parsed line*/
typedef struct data
{
    char *label;
    char *cmd;
    char **arg;
    unsigned int narg;
    unsigned int line; /*save line, as entry/extern are pushed. and the rest are enqueued*/
} data_t;
typedef struct entry
{
    char *label;
    int address;
} external_t;
typedef struct symbol
{
    char *label;
    unsigned int address;
    unsigned int command : 1;
    unsigned int external : 1;
    unsigned int entry : 1;
} symbol_t;
/*generic double linked list, data fields are changeable.*/
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
    unsigned int type : 3;
    ptr head;
    ptr tail;
} list_t;
#endif