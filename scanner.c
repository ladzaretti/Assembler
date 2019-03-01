#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data_structure.h"
#include "error.h"
#include "parser.h"
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
char *path_fname_extract(char *Fpath)
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
        printf("Allocation failed, line %d, file %s.\n", __LINE__, __FILE__);
    else
    {
        fname = temp;
        *(fname + i - 1) = 0; /*terminate string.*/
    }
    return fname;
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
        break;
    case DATA:
    case STRING:
        if (!(psymbol = create_symbol(data.label)))
            return NULL; /*if failed*/
        list_enqueue(sym_table, psymbol);
        psymbol->command = FALSE;  /*insert into symbol table. mark as data*/
        psymbol->address = DC;     /*value is current DC.*/
        psymbol->external = FALSE; /*set external as false.*/
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
        if (id == EXTERN)
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
            list_enqueue(list, (void *)pdata);     /*enqueue new data into linked list.*/
            build_symbol_type(symbol_list, pdata); /*add to symbol list if valid.*/
        }                                          /*end of symbol table creation block.*/
        free(temp);                                /*free current line string.*/
        ln_cnt++;
    }                                 /*end of input stream.*/
    symbol_table_add_IC(symbol_list); /*update address in symbol list according to IC. add IC to all addresses where external = FALSE and not a command.*/
    list_print(*symbol_list, SYMBOL_T); /*print symbol table*/
    list_print(*list, DATA_T);          /*print list*/
}