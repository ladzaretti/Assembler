#include <stdio.h>
#include "parser.h"
#include "database.h"
#define LENGTH 80
int main(int argc, char **argv)
{
    char *path_name = NULL; /*string to contain path filename*/
    char *line = NULL;
    char *label = NULL;
    char *cmd = NULL;
    list_t list;
    while (*++argv)
    {
        FILE *fp;
        fpos_t start; /*start of file keeper*/
        int ch;
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
                cmd = NULL;
                label = NULL;
                fget_line(&line, fp);
                printf("%s\n", line);
                temp = line;
                get_label_and_cmd(&line, &label, &cmd);
                puts(line);
                if (label)
                {
                    printf("%s\n", label);
                    free(label);
                }
                if (cmd)
                {
                    printf("%s\n", cmd);
                    free(cmd);
                }
                free(temp);
            }
            fclose(fp);
        }
    }
    return 0;
}