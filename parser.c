#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"
/*the following function receives a path as string and extracts its filename. the function then returns the filename as a string.*/
char *path_fname_extract(char *Fpath)
{
    int i = 0;
    char *fname = NULL; /*string to contain the filename from given path*/
    char *temp = NULL;  /*temp char pointer for allocation*/
    while (*(Fpath + i) != '.')
    {
        temp = (char *)realloc(fname, sizeof(char) * (i + 1));
        if (!temp) /*check if allocation was successful*/
        {
            puts("allocation failed");
            return NULL;
        }
        fname = temp;
        *(fname + i) = *(Fpath + i); /*copy current charecter from path.*/
        i++;
    }
    *(fname + i) = 0; /*terminate filename.*/
    return fname;
}
/*the following function removes whitespaces from the given data.*/
void remove_wspaces(char **str)
{
    int i = 0, j = 0;
    char *temp_str = NULL;
    temp_str = (char *)malloc(strlen(*str) + 1); /*cpy str to avoid src-dest overlap in strcpy inside the while loop*/
    if (!temp_str)
    {
        puts("allocation failed");
        return;
    }
    strcpy(temp_str, *str);  /*copy given string.*/
    while (*(*str + i) != 0) /*search thought the given data for white spaces.*/
    {
        if ((*(*str + i) == ' ') || (*(*str + i) == '\t')) /*whitespace search*/
        {
            strcpy(*str + i, temp_str + i + j + 1); /*overwrite whitespace with the ending substring after it*/
            j++;                                    /*counts total overwrites*/
            i--;                                    /*rewind i by one to compensate for overwritten char*/
        }
        i++; /*char checked in org string counter*/
    }
    free(temp_str); /*free copied string memory.*/
}
/*the following function gets a string of a line as an argument, returns label if exists, otherwise NULL*/
char *get_label(char **str)
{
    char *tok = NULL;
    const char del[] = ":";
    char *label = NULL;     /*string to contain the label is present.*/
    if (!strchr(*str, ':')) /*search given string for delimiter*/
        return NULL;        /*return NULL if label does not exist*/
    else
    {
        char temp[80];
        strcpy(temp, *str);
        tok = strtok(temp, del);
        label = (char *)malloc(strlen(tok) + 1); /**/
        strcpy(label, tok);
        remove_wspaces(&label);
        *str = strtok(NULL, del);
        return label;
    }
}
/*get input dynamically from stream3
function arguments: pointer to an array of char (string)
return values:
1 - finished successfully
0 - allocation failed
EOF = -1 - reached input's EOF*/
int fget_line(char **str, FILE *stream)
{
    int i = 1, ch;                                          /*ch = will get char from fgets, i = counts amount of char received, used for reallocation.*/
    char *temp = NULL;                                      /*temporary pointer to store reallocated memory block, to prevent data loss in case of failure.*/
    while (((ch = fgetc(stream)) != '\n') && ((ch != EOF))) /*get all chars from stdio till EOF / newline*/
    {
        temp = (char *)realloc(*str, sizeof(char) * i); /*allocate one more chunk of memory of size char*/
        if (!temp)
        {
            puts("allocation failed");
            return 0;
        }
        else
        {
            *str = temp;          /*give back address of the reallocated memory back to the argument pointer*/
            *(*str + i - 1) = ch; /*copy new char to the newly allocated space*/
            i++;                  /*increase counter*/
        }
    }
    /*finished copying line (if there is still data, it will be extracted in the next iteration. will reach EOF eventually)*/
    temp = (char *)realloc(*str, sizeof(char) * i); /*reallocated space for string termination*/
    if (!temp)
        puts("allocation failed");
    else
    {
        *str = temp;
        *(*str + i - 1) = 0; /*terminate string.*/
    }
    if (ch == EOF) /*return EOF if got there.*/
        return EOF;
    return 1; /*there is still date in stdio but got one line, return 1.*/
}
/*the following function receives a pointer to a string as argument.
the function returns pointer to the extracted first word in the given string.
if the given data contains no data, an empty string will be returned ("/n")*/
char *get_nxt_word(char **src)
{
    char *temp = NULL, *cmd = NULL;                               /*temp will be used to get the allocated address. cmd will be returned from the function.*/
    int i = 1;                                                    /*counts all chars in the given string argument.*/
    int j = 1;                                                    /*counts the copied chars from the input to cmd*/
    while ((*(*src + i - 1) == ' ') || (*(*src + i - 1) == '\t')) /*skip pass all starting white spaces.*/
        i++;
    if (strlen(*src) == i - 1) /*check if the entire string contains white spaces.*/
    {
        /* puts("Empty line.");*/
        return NULL;
    }
    /*the following loop will dynamically copy the first sequence of characters until reaching space, comma or null.*/
    while ((*(*src + i - 1) != ' ') && (*(*src + i - 1) != 0) && (*(*src + i - 1) != ','))
    {
        temp = (char *)realloc(cmd, sizeof(char) * j); /*same allocation as in the previous function.*/
        if (temp == NULL)
        {
            printf("Allocation Failed\n");
            return NULL;
        }
        else
        {
            cmd = temp;
            cmd[j - 1] = *(*src + i - 1); /*copy char to cmd.*/
            j++;                          /*increase cmd length counter, could initialize j as zero, wont change it now as the code works.*/
        }
        i++; /*increase data char counter.*/
    }
    if (j == 1) /*if no char was copied.*/
        return NULL;
    temp = (char *)realloc(cmd, sizeof(char) * j); /*terminate cmd with null.*/
    if (temp == NULL)
    {
        printf("Allocation Failed\n");
        return NULL;
    }
    else
    {
        cmd = temp;
        cmd[j - 1] = 0;
        *src = *src + i - 1; /*move src ptr over cmd, so the rest of the data that suppose to be function arguments can be evaluated*/
    }
    return cmd; /*return cmd address to caller*/
}
/*the following function gets a given string (line from given file), and stores its componenets in the 
coresponding field in the data structure object.
the first word is checked, if the last char of the word is ':" then the word stored as a label,otherwise as a cmd.
the rest of the line, if there is any is stored as the cmd arguments.*/
data_t *get_data(char **src)
{
    char *cmd = get_nxt_word(src);                      /*get first word.*/
    data_t *node = (data_t *)calloc(1, sizeof(data_t)); /*allocate new data obj.*/
    if (!cmd)                                           /*check if the line is empty, if so return NULL and free node.*/
    {
        free(node);
        return NULL;
    }
    if (!node)
    {
        printf("allocation failed.\n");
        return NULL;
    }
    if (cmd[strlen(cmd) - 1] == ':') /*check of the first word is a label.*/
    {
        node->label = (char *)malloc(strlen(cmd)); /*allocate space in data obj for the given label.*/
        if (!node->label)
        {
            printf("allocation failed.\n");
            return NULL;
        }
        strncpy(node->label, cmd, strlen(cmd)); /*copy the label into the data obj without the colon.*/
        (node->label)[strlen(cmd) - 1] = 0;     /*terminate at colon.*/
        free(cmd);                              /*free allocated space from utility function get_nxt_word.*/
        cmd = get_nxt_word(src);                /*get the next word.*/
    }
    if (cmd)
    {
        node->cmd = (char *)malloc(strlen(cmd) + 1);
        if (!node->cmd)
        {
            printf("allocation failed.\n");
            return NULL;
        }
        strcpy(node->cmd, cmd);
        free(cmd);
    }
    remove_wspaces(src);
    if (strlen(*src))
    {
        node->arg = (char *)malloc(strlen(*src) + 1);
        if (!node->arg)
        {
            printf("allocation failed.\n");
            return NULL;
        }
        strcpy(node->arg, *src);
    }
    return node;
}