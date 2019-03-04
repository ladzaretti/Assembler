#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"
#include "data_structure.h"
#include "error.h"
#include "parser.h"
/*address/direct word struct*/
typedef struct
{
    unsigned int are : 2; /*ARE field*/
    int data : 10;        /*address/data*/
} bin_data;
/*instruction word struct*/
typedef struct
{
    unsigned int are : 2;      /*ARE field*/
    unsigned int des_hash : 3; /*destination hashing method field*/
    unsigned int op_code : 4;  /*op code field field*/
    unsigned int src_hash : 3; /*source hashing method field*/
} bin_ins;
/*register word struct*/
typedef struct
{
    unsigned int are : 2;     /*2 bits padding*/
    unsigned int des_reg : 5; /*destination register address*/
    unsigned int src_reg : 5; /*source register address*/
} bin_reg;
typedef enum
{
    BIN_DATA = 1,
    BIN_INS,
    BIN_REG
} bin_type;
typedef enum
{
    HASH_1 = 1, /*ןmmediate*/
    HASH_3 = 3, /*direct*/
    HASH_5 = 5  /*reg*/
} hash_method;
typedef enum
{
    LINK_A = 0,
    LINK_R = 2,
    LINK_E = 1
} linkage_type;
/*array of strings of known data types.*/
const char ins_string[4][8] = {".data", ".string", ".entry", ".extern"};
/*array of strings with known commands names as defined is the assignment, to be compared using strcmp with input cmd from user.*/
const char cmd_string[16][5] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};
/*array of the system's registers.*/
const char registers[8][4] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};
/*the following function receives a string representing an integer number.
using strtod, the string is converted to a double. if the double has a decimal value other then zero, the convertion fails.
zero is returned. the vessel num doesnt changed.
if successful, TRUE is return and num gets the whole part of the double.*/
/*some functions (might have been slightly modified) are reused from assignment 22*/
/*the following function receives a path as string and extracts its filename. the function then returns the filename as a string.*/
char *path_fname_extract(const char *Fpath)
{
    int i = 1;
    char *fname = NULL; /*string to contain the filename from given path*/
    char *temp = NULL;  /*temp char pointer for allocation*/
    while (*(Fpath + i - 1) != '.')
    {
        temp = (char *)realloc(fname, sizeof(char) * i);
        if (!temp) /*check if allocation was successful*/
        {
            printf("Allocation failed, line %d, file %s.\n", __LINE__, __FILE__);
            return NULL;
        }
        else
        {
            fname = temp;
            *(fname + i - 1) = *(Fpath + i - 1); /*copy current charecter from path.*/
            i++;
        }
    }
    temp = (char *)realloc(fname, sizeof(char) * i); /*reallocated space for string termination*/
    if (!temp)
    {
        printf("Allocation failed, line %d, file %s.\n", __LINE__, __FILE__);
        return NULL;
    }
    else
    {
        fname = temp;
        *(fname + i - 1) = 0; /*terminate string.*/
    }
    return fname;
}
/*use argv to a open file, argv can contain an extension or not.
file_name will be asigned with the extensionless file name.*/
FILE *dy_fopen(const char *argv, char **file_name)
{
    int end_index = strlen(argv);
    if ((argv[end_index - 3] == '.') && (argv[end_index - 2] == 'A') && (argv[end_index - 1] == 'S'))
    {
        *file_name = path_fname_extract(argv);
        return fopen(argv, "r");
    }
    else
    {
        char *temp = NULL;
        FILE *fp = NULL;
        temp = (char *)realloc(temp, sizeof(char) * ((strlen(argv)) + 4)); /*extra for extension and null*/
        *file_name = (char *)realloc(*file_name, sizeof(char) * ((strlen(argv)) + 1));

        if ((!temp) || (!file_name)) /*check if allocation was successful*/
        {
            printf("Allocation failed, line %d, file %s.\n", __LINE__, __FILE__);
            return NULL;
        }
        strcpy(*file_name, argv);
        strcpy(temp, argv);  /*copy without null*/
        strcat(temp, ".AS"); /*add extension*/
        fp = fopen(temp, "r");
        free(temp);
        return fp;
    }
}
int get_num(char *src, int *num)
{
    char *ptr = NULL;                                                        /*pointer to receive the sting strtod didnt converted to double.*/
    double num_db = strtod(src, &ptr);                                       /*store converted double in num_db. reminder in ptr.*/
    int temp = (int)num_db;                                                  /*store the whole part of the double.*/
    if ((strlen(ptr) > 0) || (num_db != (double)temp) || (strchr(src, '.'))) /*check for conversion reminder or if the converted number has a decimal part. the last logical part can be removed in order to exepct floating point numbers witl zero as decimal.*/
        return FALSE;
    else
    {
        *num = temp; /*the whole src string is an interger.*/
        return TRUE; /*if the whole str converted to double, return true, as the given input is indeed a real number.*/
    }
}
/*the following function creates a symbol node with the given label.
input is a label string, the function then returns to the caller the address of the dynamically allocated node.
the node is initialized to 0 in all other fields.*/
symbol_t *create_symbol(char *label)
{
    symbol_t *node = (symbol_t *)calloc(1, sizeof(symbol_t)); /*allocate new data obj.*/
    if (node)
    {
        node->label = (char *)malloc(strlen(label) + 1); /*allocate space in symbol obj for the given label.*/
        if (!node->label)
        {
            printf("Allocation failed, line %d, file %s.\n", __LINE__, __FILE__);
            return NULL;
        }
        strcpy(node->label, label); /*copy the label into the symbol obj.*/
        node->address = 0;          /*initialize node fields to 0.*/
        node->command = FALSE;
        node->external = FALSE;
        node->entry = FALSE;
    }
    else
    {
        printf("Allocation failed, line %d, file %s.\n", __LINE__, __FILE__);
        return NULL;
    }
    return node;
}
/*the following function inserts into the given symbol table the given data by its type - command, data, string or extern.
inputs  - symbol table address represented by a linked list
        - data_t node by value.
        - symbol_type as the "word" type.
if successfull, new node address is return. if not null is returned.*/
symbol_t *insert_symbol(list_t *sym_table, data_t data, symbol_type type)
{
    symbol_t *psymbol = NULL; /*new symbol node.*/
    switch (type)
    {
    case COM:
        if (!(psymbol = create_symbol(data.label)))
            return NULL;                  /*if failed*/
        list_enqueue(sym_table, psymbol); /*create and enqueue symbol node with given label.*/
        psymbol->command = TRUE;          /*mark as code.*/
        psymbol->address = IC;            /*update value as current IC*/
        psymbol->external = FALSE;        /*set external as false.*/
        psymbol->entry = FALSE;
        break;
    case DATA:
    case STRING:
        if (!(psymbol = create_symbol(data.label)))
            return NULL; /*if failed*/
        list_enqueue(sym_table, psymbol);
        psymbol->command = FALSE;  /*insert into symbol table. mark as data*/
        psymbol->address = DC;     /*value is current DC.*/
        psymbol->external = FALSE; /*set external as false.*/
        psymbol->entry = FALSE;
        break;
    case EXTERN:
    {
        char **arg;
        int i = 0;
        /*insert into symbol table with extern mark.*/
        symbol_t *psymbol = NULL; /*new symbol node.*/
        /*cycle thought the different arguments*/
        for (; i < data.narg; i++)
        {
            arg = (char **)((data.arg) + i);
            if (search_label(sym_table, *arg))
                error_hndl(LABEL_EXISTS); /*if found, print error.*/
            if (!(psymbol = create_symbol(*arg)))
                return NULL;                  /*if failed*/
            list_enqueue(sym_table, psymbol); /*insert into symbol table. mark as data*/
            psymbol->command = FALSE;         /*mark as code.*/
            psymbol->address = 0;             /*set address as zero*/
            psymbol->external = TRUE;         /*set external as true.*/
            psymbol->entry = FALSE;
        }
    }
    break;
    default:
        break;
    }
    return psymbol;
}
/*the following function calculates the size in memory of a .data array /.string type.
input : - data_t as the data extracted from the current line.
        - symbol_type enum that describes the given data type -> .data/.string
return values are:  - size in memory
                    - UNINITILIZED_DATA, NON_INT or UNINITILIZED_STRING on error*/
int data_size(data_t data, symbol_type id)
{
    int i;
    int cnt = 0;
    if (id == DATA)
    {
        char **arg;
        int num;                 /*in this context, num is a dummy variable for use in get_num.
                                in the second scan, it will contain the interger extracted. */
        arg = (char **)data.arg; /*cast by pointer to get the data field*/
        if (!(data.narg))        /*check if arguments exists.*/
            return UNINITILIZED_DATA;
        for (i = 0; i < data.narg; i++) /*check and count integer arguments.*/
            if (get_num(arg[i], &num))  /*if arg is an integer*/
                cnt++;
            /*if valid, inc DC.*/
            else
            {
                error_hndl(NON_INT);
                return NON_INT;
            } /*var isnt int*/
    }
    if (id == STRING)
    {
        char **arg;
        arg = (char **)data.arg; /*cast by pointer to get the data field*/
        if ((data.narg) > 0)     /*check string for errors*/
        {
            check_string(arg);
            cnt += strlen(*arg) + 1; /*addvance DC by the length of the given string + null.*/
        }
        else
            return UNINITILIZED_STRING;
    }
    return cnt;
}
/*the following function builds a symbol entry by the given data.
the data is analyezed for its type -> instruction or command line.
then checked for errors, or if exists in the table. 
if the given data is valid, insert_symbol is used to enqueue with the corresponding parameters.
input:  - the address of the symbol list
        - the address of the data_t as extracted from the current line.
output = none*/
void build_symbol_type(list_t *symbol_list, data_t *pdata)
{
    /*the following switch cases will be executed only if there is a valid cmd/ins line. 
                        otherwise defualt case will be executed.*/
    switch (error_hndl(identify_line_type(pdata->cmd))) /*switch on line type.*/
    {
        symbol_type id;
    case (CMD_LINE):      /*command line.*/
        if (pdata->label) /*check for label.*/
        {
            if (!(search_label(symbol_list, pdata->label))) /*check if label exists in symbol table.*/
                insert_symbol(symbol_list, *pdata, COM);    /*insert into symbol table. as code*/
            else
                error_hndl(LABEL_EXISTS); /*exist, print error.*/
        }
        /*calculate L. ->  = operand_check*/
        if (pdata->narg >= 0) /*no arg error*/
            IC += (1 + error_hndl(operand_check(cmd_identify(pdata->cmd), *pdata)));
        /* IC<-L+IC increase IC by number of memory words + one words for the instruction itself.*/
        break;
    case (INS_LINE):                                                     /*instruction line.*/
        if (((id = ins_identify(pdata->cmd)) == STRING) || (id == DATA)) /*check if the given line is a data declaration.*/
        {
            if (pdata->label) /*if there is label, insert it into the symbol table.*/
            {
                if (!(search_label(symbol_list, pdata->label))) /*check if exists in symbol table.*/
                    insert_symbol(symbol_list, *pdata, id);     /*insert into symbol table. mark as non code*/
                else
                    error_hndl(LABEL_EXISTS); /*exists, print error.*/
            }
            /*update DC*/
            {
                int add;
                if ((add = data_size(*pdata, ins_identify(pdata->cmd))) > 0)
                    DC += add;
            }
        }
        if ((id == EXTERN) || (id == ENTRY))
            insert_symbol(symbol_list, *pdata, id); /*insert into symbol table with extern mark.*/
        break;
    default:
        /*line not an instruction nor a command.*/
        break;
    } /*end of switch.*/
}
/*the following function updates the given data entries 
with the final IC count.
input   - address of the symbol list
output  - none*/
void symbol_table_add_IC(list_t *symbol_list)
{
    ptr h = symbol_list->head; /*get list head*/
    symbol_t *sym;             /*pointer to the data field*/
    while (h)                  /*scan symbol table and update DC addresses by adding IC.*/
    {
        sym = h->data;
        h = h->next;
        if ((sym->external == FALSE) && (sym->command == FALSE))
            sym->address += IC;
    }
}
/*run thought parsed linked list and update the entry property in the symbol table accordingly.
input:  - parsed data linked list
        - symbol table represented as linked list.
output: none.*/
static void update_symbol_entry(list_t *list, list_t *symbol_list)
{
    node_t *p = list->head; /*head pointer*/
    while (p)
    {
        data_t *data = (data_t *)p->data;
        char **arg = (char **)data->arg;
        if (!strcmp(data->cmd, ".entry")) /*update prop if entry*/
        {
            int i;
            for (i = 0; i < data->narg; i++) /*cycle thought all arguments in the given entry line.*/
            {
                symbol_t *sym = search_label(symbol_list, *(arg + i));
                if (sym)
                    sym->entry = TRUE;
            }
        }
        if (p->next)
        {
            data = (data_t *)p->next->data;
            if ((strcmp(data->cmd, ".entry")) && (strcmp(data->cmd, ".extern")))
                return; /*entry/extern section in the list ended. as they are pushed to the list.*/
        }
        p = p->next;
    }
}
/*parse given stream in to raw list, create symbol table from the parsed list.
input:  -   addresses of two empty linked lists. one for the parsed data and the other 
            for the symbol table. 
        -   input stream.*/
void initial_scan(list_t *symbol_list, list_t *list, FILE *fp)
{
    char *line = NULL;    /*string for a line from AS file*/
    data_t *pdata = NULL; /*pointer to store data from given line, will be used as the data section in the linked list.*/
    ln_cnt = 1;           /*reset line counter before new file feed.*/
    IC = 100;
    DC = 0;
    initilize_list(list);        /*initialize a linked list*/
    initilize_list(symbol_list); /*initialize a linked list*/
    while (!feof(fp))
    {
        char *temp = NULL;              /*pointer the beginning of given line. for future free.*/
        line = NULL;                    /*initialize line pointer before each iteration*/
        fget_line(&line, fp);           /*get next line from given file.*/
        temp = line;                    /*save starting address.*/
        if (is_comment(&line, &ln_cnt)) /*check if the given line is a comment.*/
            continue;
        if ((pdata = get_data(&line))) /*if there is data, i.e the line isn't empty.*/
        {
            /*check if the given line label is valid, if ext/ent - label is ignored. if the line has label only the line is ignored.*/
            if (!check_ln_label(&pdata, &temp, &ln_cnt))
                continue;
            if (!(strcmp(pdata->cmd, ".extern")) || !(strcmp(pdata->cmd, ".entry")))
                list_push(list, (void *)pdata);
            else
                list_enqueue(list, (void *)pdata); /*enqueue new data into linked list.*/
            build_symbol_type(symbol_list, pdata); /*add to symbol list if valid.*/
        }                                          /*end of symbol table creation block.*/
        free(temp);                                /*free current line string.*/
        ln_cnt++;
    }                                       /*end of input stream.*/
    symbol_table_add_IC(symbol_list);       /*update address in symbol list according to IC. add IC to all addresses where external = FALSE and not a command.*/
    update_symbol_entry(list, symbol_list); /*update table on entry property*/
    list_print(*symbol_list, SYMBOL_T);     /*print symbol table*/
    list_print(*list, DATA_T);              /*print list*/
}
/*allocate using calloc one block of size n_byte bytes.
the function checks if allocation was successful.*/
void *ccalloc(unsigned int size, unsigned int n_byte)
{
    void *vessel = (void *)calloc(size, n_byte);
    if (vessel)
        return vessel;
    else
    {
        printf("Allocation failed, line %d, file %s.\n", __LINE__, __FILE__);
        return NULL;
    }
}
/*the following function receives a node and inserts its represention into the data list as int.
input:  - address of the linked list
        - address of a data node*/
static void insert_data_block(list_t *data_list, data_t *pdata)
{
    char **arg = (char **)pdata->arg;
    int i;
    int *int_node = NULL;
    int dat_type = ins_identify(pdata->cmd);
    if (dat_type == DATA)
    {
        /*unfoled and insert into data list*/
        for (i = 0; i < pdata->narg; i++) /*cycle thought all arguments in the given entry line.*/
        {
            int_node = ccalloc(1, sizeof(int));
            get_num(*(arg + i), int_node);     /*get int represention.*/
            list_enqueue(data_list, int_node); /*enqueue into data section*/
            DC++;
        }
    }
    else if (dat_type == STRING)
    {
        /*unfold and insert into data list*/
        for (i = 0; i < strlen(*arg); i++) /*cycle thought all characters in the given string.*/
        {
            int_node = ccalloc(1, sizeof(int));
            *int_node = *(*arg + i); /*get ASCII value*/
            list_enqueue(data_list, int_node);
            DC++;
        }
        list_enqueue(data_list, int_node = ccalloc(1, sizeof(int))); /*terminate with zero*/
        DC++;
    }
}
/*if given string is an integer, if true - create an absolute bin word.
update instruction word and return the built word to the caller.
otherwise return false.*/
bin_data *create_bin_int(char *arg, bin_ins *ins_word)
{
    bin_data *ins_info = NULL;
    int num;
    if (get_num(arg, &num)) /*if num - > insert into ins list*/
    {
        ins_info = ccalloc(1, sizeof(bin_data)); /*allocate ins_info*/
        ins_info->data = num;                    /*set val*/
        ins_info->are = LINK_A;                  /*set ARE as absolute.*/
        ins_word->des_hash = HASH_1;             /*des hash set,src hash is 0(calloc)*/
    }
    return ins_info;
}
/*create entry node from the given label and address.
returns null on failure, otherwise the address of the node.*/
external_t *build_external_node(char *label, int address)
{
    external_t *node = (external_t *)calloc(1, sizeof(external_t));
    if (!node)
    {
        printf("Allocation failed, line %d, file %s.\n", __LINE__, __FILE__);
        return NULL;
    }
    node->label = (char *)malloc(sizeof(char) * (strlen(label) + 1));
    if (!(node->label))
    {
        printf("Allocation failed, line %d, file %s.\n", __LINE__, __FILE__);
        return NULL;
    }
    else
    {
        strcpy(node->label, label); /*copy variable label*/
        node->address = address;    /*set address*/
    }
    return node;
}
/*create instruction block from the given data.
input:  - pdata as a pointer to parsed node.
        - symbol list.
        - external list address, for storing external var reference.
output  - linked list on an instruction block.*/
static list_t *build_instruction_block(list_t *symbol_list, list_t *external_list, data_t *pdata)
{
    char **arg = (char **)pdata->arg; /*get command arguments string array*/
    list_t *ins_block = ccalloc(1, sizeof(list_t));
    symbol_t *sym_data = NULL;
    int i;
    int reg_flag = FALSE;                            /*register flag, in case the both the arguments are registers*/
    bin_ins *ins_word = ccalloc(1, sizeof(bin_ins)); /*instruction word*/
    bin_data *ins_info = NULL;                       /*data word*/
    bin_reg *ins_reg = NULL;                         /*register word*/
    /*create bin_ins, code op id*/
    ins_word->op_code = cmd_identify(pdata->cmd); /*set op code*/
    list_enqueue(ins_block, (void *)ins_word);    /*enqueue instruction*/
    IC++;
    for (i = 0; i < pdata->narg; i++) /*cycle thought all arguments in the given entry line.*/
    {
        if ((sym_data = search_label(symbol_list, *(arg + i)))) /*get symbol node for the currecnt label*/
        {
            /*if label is external, linker will deal with combination errors. as we have no info on the type of the label.*/
            /*assuming that jmp,jsr and bne can operate on code only.*/
            if (((ins_word->op_code == JMP) || (ins_word->op_code == JSR)||(ins_word->op_code == BNE)) && (sym_data->external == FALSE))
            {
                if ((sym_data->command) == FALSE) /*jmp and jsr can operate on labels that points on instructions*/
                    error_hndl(VAR_AS_CMD);
            }
            else if (((sym_data->command) == TRUE) && (sym_data->external == FALSE))
                error_hndl(CMD_AS_VAR);
            /*all other commands can operate on label that points to data only*/
            ins_info = ccalloc(1, sizeof(bin_data)); /*allocate ins_info*/
            ins_info->data = sym_data->address;      /*copy label address to ins_info*/
            /*update des and src hash method*/
            if ((!i) && (pdata->narg > 1)) /*label is a source*/
                ins_word->src_hash = HASH_3;
            else /*label is a destination. narg max=2. otherwise - error in the first scan or var declaration*/
                ins_word->des_hash = HASH_3;
            if (sym_data->external == TRUE) /*the given label is external*/
            {
                external_t *node = build_external_node(*(arg + i), IC); /*insert into external list with current IC*/
                list_enqueue(external_list, (void *)node);
                ins_info->are = LINK_E; /*set ARE as external*/
            }
            else
                ins_info->are = LINK_R; /*set ARE as relocatable*/
            list_enqueue(ins_block, (void *)ins_info);
            IC++;
        }
        else if (!sym_data) /*not a label*/
        {
            if ((ins_info = create_bin_int(*(arg + i), ins_word))) /*in case the arguments is an interger*/
            {
                list_enqueue(ins_block, (void *)ins_info);
                IC++;
            }
            /*if reg - > insert into ins list, set reg flag as true*/
            else if (check_register(*(arg + i)))
            {
                if (!i) /*reg is a source*/
                {
                    ins_reg = ccalloc(1, sizeof(bin_reg));          /*allocate ins_info*/
                    ins_reg->src_reg = is_register(*(arg + i) + 1); /*+1=skip @*/
                    ins_word->src_hash = HASH_5;
                    reg_flag = TRUE;
                }
                else /*reg is a destination.narg max=2.otherwise - error in the first scan or var declaration*/
                {    /*if reg and flag is on, insert into the same word*/
                    if (reg_flag)
                    {
                        ins_reg->des_reg = is_register(*(arg + i) + 1);
                        ins_word->des_hash = HASH_5;
                        continue;
                    }
                    else
                    {
                        ins_reg = ccalloc(1, sizeof(bin_reg));
                        ins_reg->des_reg = is_register(*(arg + i) + 1);
                        ins_word->des_hash = HASH_5;
                    }
                }
                list_enqueue(ins_block, (void *)ins_reg);
                IC++;
            }
            else
            {
                if ((ins_word->op_code == JMP) || (ins_word->op_code == JSR))
                    error_hndl(UDEF_REF);
                else
                    error_hndl(UDEF_VAR);
            } /*otherwise print error = undefined variable/reference*/
        }
    }
    return ins_block;
}
/*create the binary represention of the given parsed list and the symbol table.
assumed that the input data contains no errors from the initial scan.
if encountered, update external variable list.*/
list_t *bin_translate(list_t list, list_t symbol_list, list_t **external_list)
{
    list_t *data_list = ccalloc(1, sizeof(list_t)); /*allocate list*/
    list_t *ins_list = ccalloc(1, sizeof(list_t));  /*allocate list*/
    node_t *p = list.head;                          /*get parced list head*/
    *external_list = ccalloc(1, sizeof(list_t));    /*allocate list*/
    initilize_list(data_list);
    initilize_list(ins_list);
    initilize_list(*external_list);
    IC = 100; /*initialize IC and line counter for the second scan.*/
    DC = 0;
    ln_cnt = 1;
    while ((p) && (!error())) /*loop thought the parced data*/
    {
        data_t *pdata = (data_t *)p->data;            /*get parsed data section of the current node.*/
        int ln_type = identify_line_type(pdata->cmd); /*get line type.*/
        if (ln_type == INS_LINE)                             /*node contains variable declaration.*/
            insert_data_block(data_list, (data_t *)p->data); /*insert data into data zone.*/
        if (ln_type == CMD_LINE)
        {
            list_t *temp_block = build_instruction_block(&symbol_list, *external_list, pdata); /*get instruction block*/
            chain_lists(ins_list, temp_block);                                                 /*chain new block into the instruction linked list.*/
        }
        p = p->next; /*move to the next parsed word.*/
        ln_cnt++;    /*inc line counter.*/
    }
    puts("__________________________________");
    chain_lists(ins_list, data_list); /*chain data section to the end of the instruction section*/
    return ins_list;                  /*return proccessed data.*/
}