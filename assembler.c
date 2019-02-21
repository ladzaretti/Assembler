#include <stdio.h>
#include "parser.h"
#include "database.h"
#define LENGTH 80
int main(int argc, char **argv)
{
    char *path_name = NULL; /*string to contain path filename*/
    char *line = NULL;      /*string for a line from AS file*/
    list_t list;            /*initialize a linked list to contain the raw data after the parsing process.*/
    list.head = NULL;       /*terminate the empty list*/
    list.tail = NULL;       /*terminate the empty list*/
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
                line = NULL;          /*initialize line pointer before each iteration*/
                fget_line(&line, fp); /*get next line from given file.*/
                temp = line;          /*save starting address.*/
                printf("<%s>\n", line);
                if ((pdata = get_data(&line)))                  /*if there is data, i.e the line isn't empty.*/
                    list_enqueue(&list, (void *)pdata, DATA_T); /*enqueue new data into linked list.*/
                free(temp);
            }
            list_print(list, DATA_T);
            list_free(&list, DATA_T); /*free the data list*/
            fclose(fp);
        }
    }
    return 0;
}