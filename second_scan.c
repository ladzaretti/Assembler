#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"
#include "data_structure.h"
#include "error.h"
#include "utility.h"
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
/*the following function receives a node and inserts its represention into the data list as int.
input:  - address of the linked list
        - address of a data node*/
static void insert_data_block(list_t *bin_data_list, data_t *pdata)
{
    char **arg = (char **)pdata->arg;
    int i;
    int *int_node = NULL;
    int dat_type = ins_identify(pdata->cmd);
    if (bin_data_list->type != INT_BIN_T)
    {
        printf("[%s] wrong list type\n", "insert_data_block"); /*__func__/__FUNCTION__ not supported in C90*/
        return;
    }
    if (dat_type == DATA)
    {
        /*unfoled and insert into data list*/
        for (i = 0; i < pdata->narg; i++) /*cycle thought all arguments in the given entry line.*/
        {
            int_node = ccalloc(1, sizeof(int));
            get_num(*(arg + i), int_node);         /*get int represention.*/
            list_enqueue(bin_data_list, int_node); /*enqueue into data section*/
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
            list_enqueue(bin_data_list, int_node);
            DC++;
        }
        list_enqueue(bin_data_list, int_node = ccalloc(1, sizeof(int))); /*terminate with zero*/
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
    external_t *node = (external_t *)ccalloc(1, sizeof(external_t));
    if (!(node->label = (char *)ccalloc((strlen(label) + 1), sizeof(char))))
        return NULL;
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
    list_t *bin_ins_block = ccalloc(1, sizeof(list_t));
    symbol_t *sym_data = NULL;
    int i;
    int reg_flag = FALSE;                            /*register flag, in case the both the arguments are registers*/
    bin_ins *ins_word = ccalloc(1, sizeof(bin_ins)); /*instruction word*/
    bin_data *ins_info = NULL;                       /*data word*/
    bin_reg *ins_reg = NULL;                         /*register word*/
    if (symbol_list->type != SYMBOL_T)
    {
        printf("[%s] wrong list type\n", "build_instruction_block"); /*__func__/__FUNCTION__ not supported in C90*/
        return NULL;
    }
    initilize_list(bin_ins_block, INT_BIN_T);
    /*create bin_ins, code op id*/
    ins_word->op_code = cmd_identify(pdata->cmd);  /*set op code*/
    list_enqueue(bin_ins_block, (void *)ins_word); /*enqueue instruction*/
    IC++;
    for (i = 0; i < pdata->narg; i++) /*cycle thought all arguments in the given entry line.*/
    {
        if ((sym_data = search_label(symbol_list, *(arg + i)))) /*get symbol node for the currecnt label*/
        {
            /*if label is external, linker will deal with combination errors. as we have no info on the type of the label.*/
            /*assuming that jmp,jsr and bne can operate on code only.*/
            if (((ins_word->op_code == JMP) || (ins_word->op_code == JSR) || (ins_word->op_code == BNE)) && (sym_data->external == FALSE))
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
            list_enqueue(bin_ins_block, (void *)ins_info);
            IC++;
        }
        else if (!sym_data) /*not a label*/
        {
            if ((ins_info = create_bin_int(*(arg + i), ins_word))) /*in case the arguments is an interger*/
            {
                list_enqueue(bin_ins_block, (void *)ins_info);
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
                list_enqueue(bin_ins_block, (void *)ins_reg);
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
    return bin_ins_block;
}
/*create the binary represention of the given parsed list and the symbol table.
assumed that the input data contains no errors from the initial scan.
if encountered, update external variable list.*/
list_t *bin_translate(list_t parsed_list, list_t symbol_list, list_t **external_list)
{
    list_t *bin_data_list = ccalloc(1, sizeof(list_t)); /*allocate list*/
    list_t *bin_ins_list = ccalloc(1, sizeof(list_t));  /*allocate list*/
    node_t *p = parsed_list.head;                       /*get parsed list head*/
    *external_list = ccalloc(1, sizeof(list_t));        /*allocate list*/
    if ((symbol_list.type != SYMBOL_T) || (parsed_list.type != DATA_T))
    {
        printf("[%s] wrong list type\n", "bin_translate"); /*__func__/__FUNCTION__ not supported in C90*/
        return NULL;
    }
    initilize_list(bin_data_list, INT_BIN_T);
    initilize_list(bin_ins_list, INT_BIN_T);
    initilize_list(*external_list, EXTERNAL_T);
    IC = STR_ADDRESS; /*initialize IC and line counter for the second scan.*/
    DC = 0;
    while ((p) && (!error())) /*loop thought the parced data*/
    {
        data_t *pdata = (data_t *)p->data;                       /*get parsed data section of the current node.*/
        int ln_type = identify_line_type(pdata->cmd);            /*get line type.*/
        ln_cnt = pdata->line;                                    /*as entry/extern are pushed. and the rest are enqueued. -> list is not in sync with input file. for correct error indication.*/
        if (ln_type == INS_LINE)                                 /*node contains variable declaration.*/
            insert_data_block(bin_data_list, (data_t *)p->data); /*insert data into data zone.*/
        if (ln_type == CMD_LINE)
        {
            list_t *temp_block = build_instruction_block(&symbol_list, *external_list, pdata); /*get instruction block*/
            chain_lists(bin_ins_list, temp_block);                                             /*chain new block into the instruction linked list.*/
        }
        p = p->next; /*move to the next parsed word.*/
        ln_cnt++;    /*inc line counter.*/
    }
    chain_lists(bin_ins_list, bin_data_list); /*chain data section to the end of the instruction section*/
    return bin_ins_list;                      /*return proccessed data.*/
}
/*create entry list from given symbol table*/
list_t *create_entry_list(list_t list)
{                          /*function is created to avoid printing new line in the end of the correspoding file.
if the symbol table to be scanned, and then print only entries with entry flag, the generic printing function will print many extra new lines.
basicly, all this trouble is to avoid printing extra \n in the end the output files.*/
    node_t *p = list.head; /*get symbol list head*/
    list_t *entry_list = ccalloc(1, sizeof(list_t));
    int has_entry = FALSE; /*entry list creation indicator*/
    if (!entry_list)
        return NULL;
    if (list.type != SYMBOL_T) /*check list type.*/
    {
        printf("[%s] wrong list type\n", "create_entry_list"); /*__func__/__FUNCTION__ not supported in C90*/
        return NULL;
    }
    while (p)
    {
        symbol_t *pdata = (symbol_t *)p->data;
        if (pdata->entry == TRUE)
        {
            symbol_t *new_pdata = ccalloc(1, sizeof(symbol_t));
            if (new_pdata)
            {
                has_entry = TRUE;                                                           /*set flag as true. entry list will contain atleast one node.*/
                new_pdata->label = (char *)ccalloc(strlen(pdata->label) + 1, sizeof(char)); /*copy org node to entry, copying to avoid sharing nodes between list. otherwise, freeing the lists will be tricky.*/
                strcpy(new_pdata->label, pdata->label);
                new_pdata->address = pdata->address;
            }
            list_enqueue(entry_list, (void *)new_pdata);
        }
        p = p->next;
    }
    if (has_entry)
        return entry_list;
    else
    {
        free(entry_list);
        return NULL;
    }
}
/*preform the final scan on the given parsed data.
input:  - linked list containing the parsed user input
        - a symbol list containing labels
the function creates the requierd output files to the current directory*/
void final_scan(list_t parsed_list, list_t symbol_list)
{
    list_t *entry_list = NULL;
    list_t *instruction_list = NULL; /* linked list to store the converted data.*/
    list_t *external_list = NULL;    /* linked list for storing the external variables references.*/
    if (!((parsed_list.head) && (error() == FALSE)))
        return;
    /*data exists and encountered no errors*/
    instruction_list = bin_translate(parsed_list, symbol_list, &external_list); /*second scan*/
    if (error() == FALSE)                                                       /*create output files*/
    {
        list_to_file(*instruction_list, BASE64_P, ".ob");
        list_to_file(*external_list, EXTERNAL_T, ".ext");
        if ((entry_list = create_entry_list(symbol_list))) /*if entry declared, create .ent file*/
        {
            list_to_file(*entry_list, ENTRT_P, ".ent");
            list_free(entry_list, SYMBOL_T); /*free symbol table.*/
            free(entry_list);
        }
        list_free(instruction_list, INT_BIN_T);
        list_free(external_list, EXTERNAL_T);
        free(instruction_list);
        free(external_list);
    }
}