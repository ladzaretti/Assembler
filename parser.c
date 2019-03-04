#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "database.h"
#include "error.h"
#define COMMA "," /*comma macro for future use with strtok: geting arguments tokens*/
/*the following function removes whitespaces from the given data.*/
static void remove_wspaces(char **str)
{
    int i = 0, j = 0;
    char *temp_str = NULL;
    temp_str = (char *)malloc(strlen(*str) + 1); /*cpy str to avoid src-dest overlap in strcpy inside the while loop*/
    if (!temp_str)
    {
        printf("Allocation failed, line %d, file %s.\n", __LINE__, __FILE__);
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
static void remove_leading_wspaces(char **str)
{
    int j = 0;
    char *temp_str = NULL;
    temp_str = (char *)malloc(strlen(*str) + 1); /*cpy str to avoid src-dest overlap in strcpy inside the while loop*/
    if (!temp_str)
    {
        printf("Allocation failed, line %d, file %s.\n", __LINE__, __FILE__);
        return;
    }
    strcpy(temp_str, *str);                               /*copy given string.*/
    while ((*(*str + j) == ' ') || (*(*str + j) == '\t')) /*search thought the given data for white spaces.*/
        j++;                                              /*counts total leading whitespaces*/
    strcpy(*str, temp_str + j);                           /*overwrite whitespace with the ending substring after it*/
    free(temp_str);                                       /*free copied string memory.*/
}
/*get input dynamically from stream
function arguments: pointer to an array of char (string)
return values:
TRUE - finished successfully
FALSE - allocation failed
EOF = -1 - reached input's EOF*/
int fget_line(char **str, FILE *stream)
{
    int i = 1, ch;              /*ch = will get char from fgets, i = counts amount of char received, used for reallocation.*/
    int leading_ws_flag = TRUE; /*leading whitespace flag.*/
    int reg_op_flag = FALSE;
    char *temp = NULL;                                      /*temporary pointer to store reallocated memory block, to prevent data loss in case of failure.*/
    while (((ch = fgetc(stream)) != '\n') && ((ch != EOF))) /*get all chars from stdio till EOF / newline*/
    {
        if (ch == '@')
            reg_op_flag = TRUE;
        if (((ch == ' ') || (ch == '\t')) && (leading_ws_flag)) /*skip leading whitespaces.*/
            continue;
        else if (reg_op_flag)
        {
            leading_ws_flag = TRUE;
            reg_op_flag = FALSE;
        }
        else
            leading_ws_flag = FALSE;
        temp = (char *)realloc(*str, sizeof(char) * i); /*allocate one more chunk of memory of size char*/
        if (!temp)
        {
            printf("Allocation failed, line %d, file %s.\n", __LINE__, __FILE__);
            return FALSE;
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
        printf("Allocation failed, line %d, file %s.\n", __LINE__, __FILE__);
    else
    {
        *str = temp;
        *(*str + i - 1) = 0; /*terminate string.*/
    }
    if (ch == EOF) /*return EOF if got there.*/
        return EOF;
    return TRUE; /*there is still date in the stream but got one line, return 1.*/
}
/*the following function receives a pointer to a string as argument.
the function returns pointer to the extracted first word in the given string.
if the given data contains no data, an empty string will be returned ("/n")*/
static char *get_nxt_word(char **src)
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
            printf("Allocation failed, line %d, file %s.\n", __LINE__, __FILE__);
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
        printf("Allocation failed, line %d, file %s.\n", __LINE__, __FILE__);
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
static int check_missing_comma(char *str)
{
    int i = 0, dig_alpha_seq = 0, wspace_seq = 0;
    for (; i < strlen(str); i++) /*cycle thought given string*/
    {
        if ((isalpha(*(str + i))) || (isdigit(*(str + i))) || ((*(str + i)) == '@') || ((*(str + i)) == '-')) /*search for (digit+alpha+@+-)^+*/
            dig_alpha_seq = 1;                                                                                /*if so, then flag == true*/
        else if ((!isalpha(*(str + i))) && (*(str + i) != ' ') && (!(isdigit(*(str + i)))))                   /* if next char isnt alpha/space/digit -> reset*/
            dig_alpha_seq = wspace_seq = 0;
        if ((*(str + i) == ' ') && (dig_alpha_seq)) /*if current char is space and previous seq is (digit+alpha)^+ than wspace flag is true*/
            wspace_seq = 1;
        /*if previous seq is (digit+alpha+@+-)^+(space+tab)^+ and current char is (digit+alpha) than the desired seq is found -> return true*/
        if ((dig_alpha_seq) && (wspace_seq) && ((isalpha(*(str + i))) || (isdigit(*(str + i)))))
            return TRUE;
    }
    /*didnt find seq*/
    return FALSE;
}
/*the following function receives the raw string containing the arguments got from the user.
the data is analyzed. if the raw string contains no errors (of 5 types listed below) a array of strings is returned.
in which the array elements are the arguments in chronological order (by index) as received from the user.
the function's arguments are pointer to the raw data string, pointer to an empty char matrix.
the function returns an error code if encountered any, otherwise stores the arguments in a dynamically allocated array of strings and returns the number of arguments extracted.*/
static int get_CSV_arg(char **data, char ***arg_mat)
/*return values:    ILLEGAL_COMMA = Illegal comma
                    CON_COMMA = Multiple consecutive commas
                    EXT_TEXT = Extraneous text after end of command (comma specific)
                    ALC_FAILED = allocation failed
                    MIS_COMMA = missing comma between parameters*/
{
    char *token = NULL; /**/
    char **temp = NULL;
    int i = 0; /*arguments counter*/
    remove_leading_wspaces(data);
    if (check_missing_comma(*data)) /*check for missing comma.*/
        return MIS_COMMA;
    if (**data == ',')                            /* check if the first char in the striped string is a comma*/
        return ILLEGAL_COMMA;                     /*return proper error code*/
    else if (strstr(*data, ",,"))                 /*search for consecutive of commas*/
        return CON_COMMA;                         /*if found, return proper code*/
    else if (*(*data + strlen(*data) - 1) == ',') /*check if the last char is a comma*/
        return EXT_TEXT;                          /*return error code*/
    remove_wspaces(data);                         /*remove white spaces from arguments prior phrasing.*/
    token = strtok(*data, COMMA);                 /*get first token*/
    while (token != NULL)
    {
        i++;
        temp = (char **)realloc(*arg_mat, sizeof(char *) * i); /*reallocate new block of memory for a pointer to a string*/
        if (temp == NULL)                                      /*check if allocation was successful*/
        {
            printf("Allocation failed, line %d, file %s.\n", __LINE__, __FILE__);
            return ALC_FAILED; /*return error if allocation failed*/
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
the data is analyzed. if the raw string contains no errors. a string is returned.
the function's arguments are pointer to the raw data string, pointer to an empty char matrix.
the function returns an error code if encountered one, otherwise stores the arguments in a dynamically allocated string.
return value is 1 if any string was extracted.*/
static int get_string_arg(char **data, char ***arg_mat)
/*return values:    ILLEGAL_COMMA = Illegal comma
                    EXT_TEXT = Extraneous text after end of command (comma specific)
                    ALC_FAILED = allocation failed*/
{
    char **temp = NULL;
    if (**data == ',')                                 /* check if the first char in the striped string is a comma*/
        return ILLEGAL_COMMA;                          /*return proper error code*/
    if (*(*data + strlen(*data) - 1) == ',')           /*check if the last char is a comma*/
        return EXT_TEXT;                               /*return error code*/
    remove_leading_wspaces(data);                      /*remove white spaces from arguments prior phrasing.*/
    temp = (char **)realloc(*arg_mat, sizeof(char *)); /*reallocate new block of memory for a pointer to a string*/
    if (temp == NULL)                                  /*check if allocation was successful*/
    {
        printf("Allocation failed, line %d, file %s.\n", __LINE__, __FILE__);
        return ALC_FAILED; /*return error if allocation failed*/
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
        printf("Allocation failed, line %d, file %s.\n", __LINE__, __FILE__);
        return NULL;
    }
    if (cmd[strlen(cmd) - 1] == ':') /*check of the first word is a label.*/
    {
        node->label = (char *)malloc(strlen(cmd)); /*allocate space in data obj for the given label.*/
        if (!node->label)
        {
            printf("Allocation failed, line %d, file %s.\n", __LINE__, __FILE__);
            return NULL;
        }
        strncpy(node->label, cmd, strlen(cmd)); /*copy the label into the data obj without the null terminator.*/
        (node->label)[strlen(cmd) - 1] = 0;     /*terminate at colon.*/
        error_hndl(label_check(node->label));
        free(cmd);               /*free allocated space from utility function get_nxt_word.*/
        cmd = get_nxt_word(src); /*get the next word.*/
    }
    if (cmd) /*if there is an unattended word*/
    {
        node->cmd = (char *)malloc(strlen(cmd) + 1); /*allocate space to store as cmd.*/
        if (!node->cmd)
        {
            printf("Allocation failed, line %d, file %s.\n", __LINE__, __FILE__);
            return NULL;
        }
        strcpy(node->cmd, cmd); /*copy to data_t*/
        free(cmd);              /*free cmd.*/
    }
    if ((node->cmd) && (!(strcmp(node->cmd, ".string")))) /*cmd field isnt NULL and of .string type.*/
    {
        if (strlen(*src))                                               /*if the reminder (=arguments) is non empty.*/
            error_hndl(node->narg = get_string_arg(src, &(node->arg))); /*get string and check arguments list for comma specific errors.*/
    }
    else if (strlen(*src))                                       /*if the reminder (=arguments) is non empty.*/
        error_hndl(node->narg = get_CSV_arg(src, &(node->arg))); /*get arguments and check arguments list for comma specific errors.*/
    return node;
}
