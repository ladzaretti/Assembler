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
/*print a list with given data type. the arguments are list and data type.*/
void list_print(list_t, int);
/*the following function gets a pointer to a list. the function frees it and all of its nodes. 
arg are the address of the list and the data type of its nodes.*/
void list_free(list_t *, int);
/*the following function receives a pointer to a linked list and a generic data pointer.
the data is then pushed into the start of the list within a new node.*/
void list_push(list_t *, void *);
/*the following function receives a pointer to a linked list and a generic data pointer.
the data is then enqueued into the end of the list within a new node.*/
void list_enqueue(list_t *, void *);
/*the following function creates a symbol node with the given label.
input is a label string, the function then returns to the caller the address of the dynamically allocated node.
the node is initialized to 0 in all other fields.*/
symbol_t *create_symbol_node(char *);
/*this function receives a pointer to a list and a label to search in the given list.
if the label exists, its node address is return to the caller. otherwise NULL is returned.*/
symbol_t *search_label(list_t *, char *);
#endif