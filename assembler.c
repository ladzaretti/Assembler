#include <stdio.h>
#include <stdlib.h>
#include "database.h"
#include "parser.h"
#include "err_check.h"
#define LENGTH 80
#define CMD_LINE 1
#define INS_LINE 2
int linec = 1;
int err;
int DC = 0;
int IC = 100;
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
        linec = 1;
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
                char *temp = NULL;    /*pointer the beginning of given line.*/
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
                if ((pdata = get_data(&line)) && (ign_label = ignore_label(*pdata)) != 1) /*if there is data, i.e the line isn't empty. and the line has more then label only.*/
                {
                    if (ign_label == 2) /*line contains entry/extern and a label.*/
                    {
                        free(pdata->label);  /*ignore label.*/
                        pdata->label = NULL; /*set label ptr to null.*/
                    }
                    list_enqueue(&list, (void *)pdata); /*enqueue new data into linked list.*/
                    err_invalid_cms((pdata->narg));     /*check for err for arguments list, COMMA specific.*/
                    /*check if line is instruction of command.*/
                    pdata = list.tail->data;
                    if (pdata->cmd) /*if there is a word other then a label.*/
                    {
                        ln_type = identify_line_type(pdata->cmd); /*get line type.*/
                        /*printf("<%s> err = %d\n", temp, err);*/
                        /*the following switch cases will be executed only if there is a valid cmd/ins line. 
                        otherwise defualt case will be executed.*/
                        switch (ln_type)
                        {
                            int id;
                        case (CMD_LINE): /*command line.*/
                            id = cmd_identify(pdata->cmd);
                            if (pdata->label)
                            {
                                symbol_t *psymbol = NULL;
                                if (!(search_label(&symbol_list, pdata->label))) /*check if exists in symbol table.*/
                                                                                 /*insert into symbol table. mark as code*/
                                {
                                    list_enqueue(&symbol_list, psymbol = create_symbol_node(pdata->label));
                                    psymbol->command = TRUE;
                                    psymbol->address = IC; /*value is current IC*/
                                }
                            }
                            /*calculate L.*/
                            /*IC<-L+IC*/
                            /*printf("cmd id = <%s>\n", id >= 0 ? cmd_string[id] : "-1");*/
                            break;
                        case (INS_LINE): /*instruction line.*/
                            if (((id = ins_identify(pdata->cmd)) == STRING) || (id == DATA))
                            {
                                if (pdata->label)
                                {
                                    printf("ins line, label exists ");
                                    /*check if exists.*/
                                    /*insert into symbol table. mark as data*/
                                    /*value is current DC.*/
                                }
                                /*anlyize data.*/
                                /*update DC*/
                            }
                            printf("%d\n", id);
                            if (id == EXTERN)
                            {
                                printf("ins line, extern ");
                                /*insert into symbol table with extern mark.*/
                            }
                            printf("ins id = <%s>\n", id >= 0 ? ins_string[id] : "-1");
                            break;
                        default:
                            /*line not an instruction nor command.*/
                            break;
                        }
                    }
                }
                else if (ign_label == 1) /*free current node, as its being ignored. line has label only.*/
                {
                    free(pdata->label); /*free label string*/
                    free(pdata);        /*free node*/
                    free(temp);         /*free current line string.*/
                    linec++;
                    continue; /*continue to next line.*/
                }
                free(temp); /*free current line string.*/
                linec++;
            }
            list_print(symbol_list, SYMBOL_T); /*print symbol table*/
            list_print(list, DATA_T);          /*print list*/
            list_free(&symbol_list, SYMBOL_T); /*free symbol table.*/
            list_free(&list, DATA_T);          /*free the data list*/
            fclose(fp);
        }
        free(path_name);
    }
    return 0;
}