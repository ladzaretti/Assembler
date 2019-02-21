#include <stdio.h>
#include "parser.h"
#include "database.h"
#define LENGTH 80
int main(int argc, char **argv)
{
    char *path_name = NULL; /*string to contain path filename*/
    char *line = NULL;      /*string for a line from AS file*/
    list_t list;
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
                node_t elm;
                line = NULL;
                fget_line(&line, fp);
                temp = line;
                printf("<%s>\n", line);
                elm.data = get_data(&line);
                print_data(elm.data, DATA_T);
                free(temp);
                free_data(&elm.data, DATA_T);
            }
            fclose(fp);
        }
    }
    return 0;
}