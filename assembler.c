/*please be advised, the following implementation supports multiply arguments for 
.extern and .entry declaration. (i.e ".extern/.entry x,y,z" is allowed as long they are declared)*/
/*the entire project is done dynamically. therefore line length or the size or the memory needed are not checked. 
if the machine's limits are exceeded, the allocations will fail */
#include <stdio.h>
#include <stdlib.h>
#include "database.h"
#include "utility.h"
#include "scanner.h"
#include "data_structure.h"
char *file_name = NULL; /*string to contain path filename*/
int ln_cnt = 1;         /*line counter.*/
int DC = 0;             /*data counter.*/
int IC = STR_ADDRESS;   /*instruction counter.*/
int main(int argc, char **argv)
{
    list_t parsed_list; /* linked list to contain the parsed data.*/
    list_t symbol_list; /* linked list to contain the the symbol table.*/
    while (*++argv)
    {
        FILE *fp;
        file_name = path_fname_extract(*argv);
        if (!(fp = dyn_fopen(*argv, &file_name))) /*dynamically open input file (with/without extension)*/
        {
            printf("file <%s> does not exist.\n", file_name);
            free(file_name);
            return 0;
        }
        initial_scan(&symbol_list, &parsed_list, fp); /*first scan*/
        final_scan(parsed_list, symbol_list);         /*second scan*/
        /*free lists*/
        list_free(&symbol_list, SYMBOL_T); /*free symbol table.*/
        list_free(&parsed_list, DATA_T);   /*free the data list*/
        free(file_name);
        fclose(fp);
    }
    return 0;
}