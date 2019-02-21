#include <stdio.h>
#include "parser.h"
#include "database.h"
#define LENGTH 80
int main(int argc, char **argv)
{
    char *path_name = NULL; /*string to contain path filename*/
    char *line = NULL;      /*string for a line from AS file*/
    list_t list;
    list.head = NULL;
    list.tail = NULL;
    while (*++argv)
    {
        FILE *fp;
        fpos_t start; /*start of file keeper*/
        /*first file scan*/
        if (!(fp = fopen(*argv, "r")))
            printf("\n%s file does not exists.", *argv);
        else
        {
            fgetpos(fp, &start);
            while (!feof(fp))
            {
                char *temp = NULL;
                line = NULL;
                fget_line(&line, fp);
                temp = line;
                printf("<%s>\n", line);
                list_enqueue(&list, (void *)get_data(&line), DATA_T);
                free(temp);
            }
            list_print(list, DATA_T);
            list_free(&list, DATA_T);
            fclose(fp);
        }
    }
    return 0;
}