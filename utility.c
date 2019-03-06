#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
