#include <stdio.h>
#include "database.h"
#include "parser.h"
#include "err_check.h"
#define LENGTH 80
int linec = 1;
int err;
int main(int argc, char **argv)
{
    char *path_name = NULL; /*string to contain path filename*/
    char *line = NULL;      /*string for a line from AS file*/
    list_t list;            /*initialize a linked list to contain the raw data after the parsing process.*/
    list.head = NULL;       /*terminate the empty list*/
    list.tail = NULL;       /*terminate the empty list*/
    linec = 0;
    while (*++argv)
    {
        FILE *fp;
        fpos_t start; /*start of file keeper*/
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
                    /*printf("err=%d\n", err);*/
                    if (pdata->cmd)
                    {
                        int id;
                        printf("ins id = <%s> %d\n", (id = ins_identify(pdata->cmd)) >= 0 ? ins_string[id] : "-1", id);
                        printf("cmd id = <%s> %d\n\n", (id = cmd_identify(pdata->cmd)) >= 0 ? cmd_string[id] : "-1", id);
                    }
                }
                printf("<%s> err = %d\n", temp, err);
                if (ign_label == 1) /*free current node, as its being ignored.*/
                {
                    free(pdata->label);
                    free(pdata);
                }
                free(temp); /*free current line string.*/
                linec++;
            }
            list_print(list, DATA_T); /*print list*/
            list_free(&list, DATA_T); /*free the data list*/
            fclose(fp);
        }
    }
    return 0;
}