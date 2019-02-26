#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "database.h"
#include "err_check.h"
#define COMMA "," /*comma macro for future use with strtok: geting arguments tokens*/
/*some functions (might have been slightly modified) are reused from assignment 22*/
/*the following function receives a path as string and extracts its filename. the function then returns the filename as a string.*/
char *path_fname_extract(char *Fpath)
{
    int i = 1;
    char *fname = NULL; /*string to contain the filename from given path*/
    char *temp = NULL;  /*temp char pointer for allocation*/
    while (*(Fpath + i - 1) != '.')
    {
        temp = (char *)realloc(fname, sizeof(char) * i);
        if (!temp) /*check if allocation was successful*/
        {
            puts("allocation failed");
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
        puts("allocation failed");
    else
    {
        fname = temp;
        *(fname + i - 1) = 0; /*terminate string.*/
    }
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
/*the following function removes leading whitespaces from the given data.*/
void remove_leading_wspaces(char **str)
{
    int j = 0;
    char *temp_str = NULL;
    temp_str = (char *)malloc(strlen(*str) + 1); /*cpy str to avoid src-dest overlap in strcpy inside the while loop*/
    if (!temp_str)
    {
        puts("allocation failed");
        return;
    }
    strcpy(temp_str, *str);                               /*copy given string.*/
    while ((*(*str + j) == ' ') || (*(*str + j) == '\t')) /*search thought the given data for white spaces.*/
        j++;                                              /*counts total leading whitespaces*/
    strcpy(*str, temp_str + j);                           /*overwrite whitespace with the ending substring after it*/
    free(temp_str);                                       /*free copied string memory.*/
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
    int leading_ws_flag = 1;                                /*leading whitespace flag.*/
    char *temp = NULL;                                      /*temporary pointer to store reallocated memory block, to prevent data loss in case of failure.*/
    while (((ch = fgetc(stream)) != '\n') && ((ch != EOF))) /*get all chars from stdio till EOF / newline*/
    {
        if (((ch == ' ') || (ch == '\t')) && (leading_ws_flag)) /*skip leading whitespaces.*/
            continue;
        else
            leading_ws_flag = 0;
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
    return 1; /*there is still date in the stream but got one line, return 1.*/
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
/*the following function receives a string as argument, then it searches for a sequence of (digit+alpha)^+(space+tab)^+(digit+alpha) (regex)
if found one, the function returns true, otherwise false.*/
int check_missing_comma(char *str)
{
    int i = 0, dig_alpha_seq = 0, wspace_seq = 0;
    for (; i < strlen(str); i++) /*cycle thought given string*/
    {
        if ((isalpha(*(str + i))) || (isdigit(*(str + i))))                                 /*search for (digit+alpha)^+*/
            dig_alpha_seq = 1;                                                              /*if so, then flag == true*/
        else if ((!isalpha(*(str + i))) && (*(str + i) != ' ') && (!(isdigit(*(str + i))))) /* if next char isnt alpha/space/digit -> reset*/
            dig_alpha_seq = wspace_seq = 0;
        if ((*(str + i) == ' ') && (dig_alpha_seq)) /*if current char is space and previous seq is (digit+alpha)^+ than wspace flag is true*/
            wspace_seq = 1;
        /*if previous seq is (digit+alpha)^+(space+tab)^+ and current char is (digit+alpha) than the desired seq is found -> return true*/
        if ((dig_alpha_seq) && (wspace_seq) && ((isalpha(*(str + i))) || (isdigit(*(str + i)))))
            return 1;
    }
    /*didnt find seq*/
    return 0;
}
/*the following function receives the raw string containing the arguments got from the user.
the data is analyzed. if the raw string contains no errors (of 5 types listed below) a array of strings is returned.
in which the array elements are the arguments in chronological order (by index) as received from the user.
the function's arguments are pointer to the raw data string, pointer to an empty char matrix.
the function returns an error code if encountered any, otherwise stores the arguments in a dynamically allocated array of strings and returns the number of arguments extracted.*/
int get_CSV_arg(char **data, char ***arg_mat)
/*return values:    -1 = Illegal comma
                    -2 = Multiple consecutive commas
                    -3 = Extraneous text after end of command (comma specific)
                    -4 = allocation failed
                    -5 = missing comma between parameters*/
{
    char *token = NULL; /**/
    char **temp = NULL;
    int i = 0; /*arguments counter*/
    remove_leading_wspaces(data);
    if (check_missing_comma(*data)) /*check for missing comma.*/
        return -5;
    if (**data == ',')                            /* check if the first char in the striped string is a comma*/
        return -1;                                /*return proper error code*/
    else if (strstr(*data, ",,"))                 /*search for consecutive of commas*/
        return -2;                                /*if found, return proper code*/
    else if (*(*data + strlen(*data) - 1) == ',') /*check if the last char is a comma*/
        return -3;                                /*return error code*/
    remove_wspaces(data);                         /*remove white spaces from arguments prior phrasing.*/
    token = strtok(*data, COMMA); /*get first token*/
    while (token != NULL)
    {
        i++;
        temp = (char **)realloc(*arg_mat, sizeof(char *) * i); /*reallocate new block of memory for a pointer to a string*/
        if (temp == NULL)                                      /*check if allocation was successful*/
        {
            printf("Allocation Failed\n");
            return -4; /*return error if allocation failed*/
        }
        else
        {
            *arg_mat = temp;                                         /*give memory block back if allocation was successful*/
            *(*arg_mat + i - 1) = (char *)malloc(strlen(token) + 1); /*allocate new space for saving token*/
            strcpy(*(*arg_mat + i - 1), token);                      /*copy token to newly allocated memory space*/
        }
        token = strtok(NULL, COMMA); /*find next token*/
    }
    return i; /*return number of arguments received*/
}
/*the following function receives the raw string containing the arguments got from the user.
the data is analyzed. if the raw string contains no errors (of 5 types listed below) a array of strings is returned.
in which the array elements are the arguments in chronological order (by index) as received from the user.
the function's arguments are pointer to the raw data string, pointer to an empty char matrix.
the function returns an error code if encountered any, otherwise stores the arguments in a dynamically allocated array of strings and returns the number of arguments extracted.*/
int get_string_arg(char **data, char ***arg_mat)
/*return values:    -1 = Illegal comma
                    -3 = Extraneous text after end of command (comma specific)
                    -4 = allocation failed*/
{
    char **temp = NULL;
    if (**data == ',')                                 /* check if the first char in the striped string is a comma*/
        return -1;                                     /*return proper error code*/
    if (*(*data + strlen(*data) - 1) == ',')           /*check if the last char is a comma*/
        return -3;                                     /*return error code*/
    remove_leading_wspaces(data);                      /*remove white spaces from arguments prior phrasing.*/
    temp = (char **)realloc(*arg_mat, sizeof(char *)); /*reallocate new block of memory for a pointer to a string*/
    if (temp == NULL)                                  /*check if allocation was successful*/
    {
        printf("Allocation Failed\n");
        return -4; /*return error if allocation failed*/
    }
    else
    {
        *arg_mat = temp;                                 /*give memory block back if allocation was successful*/
        *(*arg_mat) = (char *)malloc(strlen(*data) + 1); /*allocate new space for saving token*/
        strcpy(*(*arg_mat), *data);                      /*copy token to newly allocated memory space*/
    }
    return 1; /*return 1 as one string was extracted*/
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
        strncpy(node->label, cmd, strlen(cmd)); /*copy the label into the data obj without the null terminator.*/
        (node->label)[strlen(cmd) - 1] = 0;     /*terminate at colon.*/
        label_check(node->label);
        free(cmd);               /*free allocated space from utility function get_nxt_word.*/
        cmd = get_nxt_word(src); /*get the next word.*/
    }
    if (cmd) /*if there is an unattended word*/
    {
        node->cmd = (char *)malloc(strlen(cmd) + 1); /*allocate space to store as cmd.*/
        if (!node->cmd)
        {
            printf("allocation failed.\n");
            return NULL;
        }
        strcpy(node->cmd, cmd); /*copy to data_t*/
        free(cmd);              /*free cmd.*/
    }
    if ((node->cmd) && (!(strcmp(node->cmd, ".string")))) /*cmd field isnt NULL and of .string type.*/
    {
        if (strlen(*src)) /*if the reminder (=arguments) is non empty.*/
            node->narg = get_string_arg(src, &(node->arg));
    }
    else if (strlen(*src)) /*if the reminder (=arguments) is non empty.*/
        node->narg = get_CSV_arg(src, &(node->arg));
    return node;
}
/*the following function receives a string representing an integer number.
using strtod, the string is converted to a double. if the double has a decimal value other then zero, the convertion fails.
zero is returned. the vessel num doesnt changed.
if successful, 1 is return and num gets the whole part of the double.*/
int get_num(char *src, int *num)
{
    char *ptr = NULL;                                                        /*pointer to receive the sting strtod didnt converted to double.*/
    double num_db = strtod(src, &ptr);                                       /*store converted double in num_db. reminder in ptr.*/
    int temp = (int)num_db;                                                  /*store the whole part of the double.*/
    if ((strlen(ptr) > 0) || (num_db != (double)temp) || (strchr(src, '.'))) /*check for conversion reminder or if the converted number has a decimal part. the last logical part can be removed in order to exepct floating point numbers witl zero as decimal.*/
        return 0;
    else
    {
        *num = temp; /*the whole src string is an interger.*/
        return 1;    /*if the whole str converted to double, return true, as the given input is indeed a real number.*/
    }
}