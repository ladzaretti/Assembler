#ifndef DATABASE_H
#define DATABASE_H
#define CMD_LINE 1 /*define command line ID*/
#define INS_LINE 2 /*define instruction line ID.*/
#define TRUE 1
#define FALSE 0
#define REG_NUM 7
#define LABEL_MAX_LEN 31
extern const char cmd_string[16][5];
extern const char ins_string[4][8];
extern const char registers[8][4];
extern int linec; /*read line counter.*/
extern int err;   /*global error flag*/
extern int IC;
extern int DC;
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
    EXTERN = 3
} instruction;
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