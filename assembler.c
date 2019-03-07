/*please use LF as an EOL on input files*/
/*please be advised, the following implemention supports multiply arguments for 
.extern and .entry declaration. (i.e ".extern/.entry x,y,z" is allowed)*/
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
    list_t parsed_list; /* linked list to contain the prased data.*/
    list_t symbol_list; /* linked list to contain the the symbol table.*/
    while (*++argv)
    {
        FILE *fp;
        file_name = path_fname_extract(*argv);
        if (!(fp = dyn_fopen(*argv, &file_name))) /*dynamicaly open input file (with/without .AS extension)*/
        {
            printf("file <%s> does not exist.\n", file_name);
            return 0;
        }
        initial_scan(&symbol_list, &parsed_list, fp); /*first scan*/
        /*fprint_list(stdout, parsed_list, DATA_T);
        fprint_list(stdout, symbol_list, SYMBOL_T);*/
        final_scan(parsed_list, symbol_list); /*second scan*/
        /*free lists*/
        list_free(&symbol_list, SYMBOL_T); /*free symbol table.*/
        list_free(&parsed_list, DATA_T);   /*free the data list*/
        free(file_name);
        fclose(fp);
    } /*while (*argv) block.*/
    return 0;
}