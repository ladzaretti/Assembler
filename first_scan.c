#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"
#include "data_structure.h"
#include "error.h"
#include "parser.h"
#include "utility.h"
/*array of strings of known data types.*/
const char ins_string[4][8] = {"data", "string", "entry", "extern"};
/*array of strings with known commands names as defined is the assignment, to be compared using strcmp with input cmd from user.*/
const char cmd_string[16][5] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};
/*array of the system's registers.*/
const char registers[8][4] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};
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
    if (sym_table->type != SYMBOL_T)
    {
        printf("[%s] wrong list type\n", "insert_symbol"); /*__func__/__FUNCTION__ not supported in C90*/
        return NULL;
    }
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
    if (symbol_list->type != SYMBOL_T)
    {
        printf("[%s] wrong list type\n", "build_symbol_type"); /*__func__/__FUNCTION__ not supported in C90*/
        return;
    }
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
    if (symbol_list->type != SYMBOL_T)
    {
        printf("[%s] wrong list type\n", "symbol_table_add_IC"); /*__func__/__FUNCTION__ not supported in C90*/
        return;
    }
    while (h) /*scan symbol table and update DC addresses by adding IC.*/
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
static void update_symbol_entry(list_t *parsed_list, list_t *symbol_list)
{
    node_t *p = parsed_list->head; /*head pointer*/
    if ((symbol_list->type != SYMBOL_T) || (parsed_list->type != DATA_T))
    {
        printf("[%s] wrong list type\n", "update_symbol_entry"); /*__func__/__FUNCTION__ not supported in C90*/
        return;
    }
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
void initial_scan(list_t *symbol_list, list_t *parsed_list, FILE *fp)
{
    char *line = NULL;    /*string for a line from AS file*/
    data_t *pdata = NULL; /*pointer to store data from given line, will be used as the data section in the linked list.*/
    ln_cnt = 1;           /*reset line counter before new file feed.*/
    IC = STR_ADDRESS;
    DC = 0;
    reset_error(); /*reset error flag before scanning new input file*/
    initilize_list(parsed_list, DATA_T);   /*initialize a linked list*/
    initilize_list(symbol_list, SYMBOL_T); /*initialize a linked list*/
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
                list_push(parsed_list, (void *)pdata);
            else
                list_enqueue(parsed_list, (void *)pdata); /*enqueue new data into linked list.*/
            build_symbol_type(symbol_list, pdata);        /*add to symbol list if valid.*/
        }                                                 /*end of symbol table creation block.*/
        free(temp);                                       /*free current line string.*/
        ln_cnt++;
    }                                              /*end of input stream.*/
    symbol_table_add_IC(symbol_list);              /*update address in symbol list according to IC. add IC to all addresses where external = FALSE and not a command.*/
    update_symbol_entry(parsed_list, symbol_list); /*update table on entry property*/
}