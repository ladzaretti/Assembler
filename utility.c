#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define FALSE 0
#define TRUE 1
/*the following function receives a path as string and extracts its filename. the function then returns the filename as a string.*/
char *path_fname_extract(const char *Fpath)
{
    int i = 1;
    char *fname = NULL; /*string to contain the filename from given path*/
    char *temp = NULL;  /*temp char pointer for allocation*/
    while (*(Fpath + i - 1) != '.')
    {
        temp = (char *)realloc(fname, sizeof(char) * i);
        if (!temp) /*check if allocation was successful*/
        {
            printf("Allocation failed, line %d, file %s.\n", __LINE__, __FILE__);
            return NULL;
        }
        else
        {
            fname = temp;
            *(fname + i - 1) = *(Fpath + i - 1); /*copy current charecter from path.*/
            i++;
        }
    }
    temp = (char *)realloc(fname, sizeof(char) * i); /*reallocated space for string termination*/
    if (!temp)
    {
        printf("Allocation failed, line %d, file %s.\n", __LINE__, __FILE__);
        return NULL;
    }
    else
    {
        fname = temp;
        *(fname + i - 1) = 0; /*terminate string.*/
    }
    return fname;
}
/*calloc with allocation check.
first arg - blocks to allocate.
second arg - block size.*/
void *ccalloc(unsigned int size, unsigned int n_byte)
{
    void *vessel = (void *)calloc(size, n_byte);
    if (vessel)
        return vessel;
    else
    {
        printf("Allocation failed, line %d, file %s.\n", __LINE__, __FILE__);
        return NULL;
    }
}
/*concatenate the given strings into a new string without changing the input.
input - first argument is the beginning, the second string is the ending.
return pointer to a new allocated string containing the result.*/
char *strcat_new(const char *str, const char *end)
{
    char *cat = (char *)ccalloc(strlen(str) + strlen(end) + 1, sizeof(char));
    if (cat)
    {
        strcpy(cat, str);
        strcat(cat, end);
    }
    return cat;
}
/*use argv to a open file, argv can contain an extension or not.
file_name will be assigned with the extension-less file name.
also, extension is non case sensitive*/
FILE *dyn_fopen(const char *argv, char **file_name)
{
    int end_index = strlen(argv);
    if ((argv[end_index - 3] == '.') && ((argv[end_index - 2] == 'A') || (argv[end_index - 2] == 'a')) && ((argv[end_index - 1] == 'S') || (argv[end_index - 1] == 's')))
        return fopen(argv, "r");
    else
    {
        char *temp = NULL;
        FILE *fp = NULL;
        temp = (char *)realloc(temp, sizeof(char) * ((strlen(argv)) + 4)); /*extra for extension and null*/
        *file_name = (char *)realloc(*file_name, sizeof(char) * ((strlen(argv)) + 1));

        if ((!temp) || (!file_name)) /*check if allocation was successful*/
        {
            printf("Allocation failed, line %d, file %s.\n", __LINE__, __FILE__);
            return NULL;
        }
        strcpy(*file_name, argv);
        strcpy(temp, argv);  /*copy without null*/
        strcat(temp, ".as"); /*add extension*/
        fp = fopen(temp, "r");
        free(temp);
        return fp;
    }
}
/*the following function receives a string representing an integer number.
using strtod, the string is converted to a double. if the double has a decimal value other then zero, the conversion fails.
zero is returned. the vessel num doesnt changed.
if successful, TRUE is return and num gets the whole part of the double.*/
int get_num(char *src, int *num)
{
    char *ptr = NULL;                                                        /*pointer to receive the sting strtod didnt converted to double.*/
    double num_db = strtod(src, &ptr);                                       /*store converted double in num_db. reminder in ptr.*/
    int temp = (int)num_db;                                                  /*store the whole part of the double.*/
    if ((strlen(ptr) > 0) || (num_db != (double)temp) || (strchr(src, '.'))) /*check for conversion reminder or if the converted number has a decimal part. the last logical part can be removed in order to exepct floating point numbers witl zero as decimal.*/
        return FALSE;
    else
    {
        *num = temp; /*the whole src string is an integer.*/
        return TRUE; /*if the whole str converted to double, return true, as the given input is indeed a real number.*/
    }
}