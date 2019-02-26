#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"
#include "parser.h"
#include "err_check.h"
#include "data_structure.h"
int linec = 1; /*line counter.*/
int err;       /*error flag.*/
int DC = 0;    /*data counter.*/
int IC = 100;  /*instruction counter.*/
int main(int argc, char **argv)
{
    char *path_name = NULL; /*string to contain path filename*/
    list_t list;            /*initialize a linked list to contain the raw data after the parsing process.*/
    list_t symbol_list;     /*initialize a linked list to contain the the symbol table.*/
    while (*++argv)
    {
        FILE *fp;
        fpos_t start;            /*start of file keeper*/
        char *line = NULL;       /*string for a line from AS file*/
        list.head = NULL;        /*terminate the empty list*/
        list.tail = NULL;        /*terminate the empty list*/
        symbol_list.head = NULL; /*terminate the empty list*/
        symbol_list.tail = NULL; /*terminate the empty list*/
        linec = 1;               /*reset line counter before new line.*/
        path_name = path_fname_extract(*argv);
        printf("file=<%s>\n\n", path_name);
        /*first file scan*/
        if (!(fp = fopen(*argv, "r")))
            printf("\n%s file does not exists.", *argv);
        else
        {
            data_t *pdata = NULL; /*pointer to store data from given line, will be used as the data section in the linked list.*/
            fgetpos(fp, &start);  /*save start of file pos, for the second scan.*/
            while (!feof(fp))
            {
                char *temp = NULL;    /*pointer the beginning of given line. for future free.*/
                int ign_label = 0;    /*lable/line ignore flag.*/
                int ln_type;          /*type of line indicator.*/
                line = NULL;          /*initialize line pointer before each iteration*/
                fget_line(&line, fp); /*get next line from given file.*/
                temp = line;          /*save starting address.*/
                if (line[0] == ';')   /*check if the given line is a comment.*/
                {
                    linec++;
                    free(temp);
                    continue; /*skip line.*/
                }
                if ((pdata = get_data(&line))) /*if there is data, i.e the line isn't empty.*/
                {
                    ign_label = ignore_label(*pdata); /*get label status. 1 = line has label only, 2 = ext/ent with label, 0 = label is valid.*/
                    if (ign_label == 1)               /*free current node, as its being ignored. line has label only.*/
                    {
                        free(pdata->label); /*free label string*/
                        free(pdata);        /*free node*/
                        free(temp);         /*free current line string.*/
                        linec++;            /*printf("<%s> err = %d\n", temp, err);*/
                        continue;           /*continue to next line.*/
                    }
                    if (ign_label == 2) /*line contains entry/extern and a label.*/
                    {
                        free(pdata->label);  /*ignore label.*/
                        pdata->label = NULL; /*set label ptr to null.*/
                    }
                    list_enqueue(&list, (void *)pdata); /*enqueue new data into linked list.*/
                    err_invalid_cms((pdata->narg));     /*check arguments list for comma specific errors.*/
                    /*check if line is an instruction or a command.*/
                    pdata = list.tail->data; /*get address of the VOID * data section*/
                    if (pdata->cmd)          /*if there is a word other then a label.*/
                    {
                        ln_type = identify_line_type(pdata->cmd); /*get line type.*/
                        /*printf("<%s> err = %d\n", temp, err);*/
                        /*the following switch cases will be executed only if there is a valid cmd/ins line. 
                        otherwise defualt case will be executed.*/
                        switch (ln_type)
                        {
                            int id, i;
                        case (CMD_LINE):                   /*command line.*/
                            id = cmd_identify(pdata->cmd); /*get the num id of the given cmd.*/
                            if (pdata->label)              /*check for label.*/
                            {
                                symbol_t *psymbol = NULL;                        /*new symbol node.*/
                                if (!(search_label(&symbol_list, pdata->label))) /*check if label exists in symbol table.*/
                                                                                 /*insert into symbol table. mark as code*/
                                {
                                    list_enqueue(&symbol_list, psymbol = create_symbol_node(pdata->label)); /*create and enqueue symbol node with given label.*/
                                    psymbol->command = TRUE;                                                /*mark as code.*/
                                    psymbol->address = IC;                                                  /*update value as current IC*/
                                    psymbol->external = FALSE;                                              /*set external as false.*/
                                }
                                else
                                    printf("error: <%s> - label already exists [line %d]\n", pdata->label, linec); /*if found, print error.*/
                            }
                            /*calculate L. ->  = cmd_operand_check*/
                            IC += 1 + cmd_operand_check(id, *pdata); /* IC<-L+IC increase IC by number of memory words + one words for the instruction itself.*/
                            break;
                        case (INS_LINE):                                                     /*instruction line.*/
                            if (((id = ins_identify(pdata->cmd)) == STRING) || (id == DATA)) /*check if the given line is a data declaration.*/
                            {
                                if (pdata->label)
                                {
                                    symbol_t *psymbol = NULL;                        /*new symbol node.*/
                                    if (!(search_label(&symbol_list, pdata->label))) /*check if exists in symbol table.*/
                                                                                     /*insert into symbol table. mark as code*/
                                    {
                                        list_enqueue(&symbol_list, psymbol = create_symbol_node(pdata->label)); /*create and enqueue symbol node with given label.*/
                                        psymbol->command = FALSE;                                               /*insert into symbol table. mark as data*/
                                        psymbol->address = DC;                                                  /*value is current DC.*/
                                        psymbol->external = FALSE;                                              /*set external as false.*/
                                    }
                                    else
                                        printf("error: <%s> - label already exists [line %d]\n", pdata->label, linec); /*if found, print error.*/
                                }
                                /*update DC*/
                                if (id == DATA)
                                {
                                    char **arg;
                                    int num;          /*in this context, num is a dummy variable for use in get_num.
                                    in the second scan, it will contain the interger extracted. */
                                    arg = pdata->arg; /*cast by pointer to get the data field*/
                                    if (!(pdata->narg))
                                    {
                                        printf("error: uninitilaied .data variable [line %d]\n", linec);
                                        err = TRUE;
                                    }                                 /*if found, print error.*/
                                    for (i = 0; i < pdata->narg; i++) /*check and count integer arguments.*/
                                        if (get_num(arg[i], &num))
                                            DC++; /*if valid, inc DC.*/
                                        else
                                        {
                                            printf("error: <%s> - is not an integer [line %d]\n", arg[i], linec); /*if found, print error.*/
                                            err = TRUE;
                                        }
                                }
                                if (id == STRING)
                                {
                                    char **arg;
                                    arg = pdata->arg;      /*cast by pointer to get the data field*/
                                    if ((pdata->narg) > 0) /*check string for errors*/
                                    {
                                        check_string(arg);
                                        DC += strlen(*arg) + 1; /*addvance DC by the lenght of the given string + null.*/
                                    }
                                    else
                                    {
                                        printf("error: uninitilaied .string variable [line %d]\n", linec);
                                    }
                                }
                            }
                            if (id == EXTERN)
                            {
                                char **arg;
                                int i = 0;
                                /*insert into symbol table with extern mark.*/
                                symbol_t *psymbol = NULL; /*new symbol node.*/
                                /**/
                                for (; i < pdata->narg; i++)
                                {
                                    arg = (pdata->arg) + i;
                                    if (search_label(&symbol_list, *arg))
                                    {
                                        printf("error: <%s> - label already exists [line %d]\n", *arg, linec);
                                        err = TRUE;
                                    }                                                               /*if found, print error.*/
                                    list_enqueue(&symbol_list, psymbol = create_symbol_node(*arg)); /*create and enqueue symbol node with given label.*/
                                    psymbol->command = FALSE;                                       /*mark as code.*/
                                    psymbol->address = 0;                                           /*set address as zero*/
                                    psymbol->external = TRUE;
                                } /*set external as true.*/
                            }
                            /*printf("ins id = <%s>\n", id >= 0 ? ins_string[id] : "-1");*/
                            break;
                        default:
                            /*line not an instruction nor a command.*/
                            break;
                        } /*end of switch.*/
                    }
                } /*end of symbol table creation block.*/
                /*update address in symbol list according to IC. add IC-100+1 to all addresses where external = FALSE*/
                free(temp); /*free current line string.*/
                linec++;
            } /*end of input stream.*/
            {
                ptr h = symbol_list.head;
                symbol_t *sym;
                while (h)
                {
                    sym = h->data;
                    h = (h)->next;
                    if ((sym->external == FALSE) && (sym->command == FALSE))
                        sym->address += IC;
                }
            }
            list_print(symbol_list, SYMBOL_T); /*print symbol table*/
            list_print(list, DATA_T);          /*print list*/
            list_free(&symbol_list, SYMBOL_T); /*free symbol table.*/
            list_free(&list, DATA_T);          /*free the data list*/
            fclose(fp);
        }
        free(path_name);
    } /*while (*argv) block.*/
    return 0;
}