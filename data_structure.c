#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "database.h"
#define WORD_SIZE 12
/*calloc with allocation check.*/
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
/*initilize linked list with NULL.
input - address of a list.*/
void initilize_list(list_t *list, unsigned int type)
{
    list->type = type;
    list->head = NULL; /*terminate the empty list*/
    list->tail = NULL; /*terminate the empty list*/
}
/*the following function frees an dynamically allocated array of strings.
arguments are pointer to an array of strings, and size of the array (number of strings stored).*/
static void free_CSV_arg(char ***arg_mat, int arg_num)
{
    int i = 0;
    for (; i < arg_num; i++)
        free(*(*arg_mat + i)); /*free dynamically allocated strings inside the matrix*/
    free(*arg_mat);            /*free string array, dynamically allocated in line 134*/
}
/*the following function receives a address of a data_t node, then frees its allocated fields. and finally frees the node itself.*/
static void free_dt(void **node)
{
    data_t **p = (data_t **)node;
    if ((*p)->label)       /*if field is non NULL.*/
        free((*p)->label); /*free field.*/
    if ((*p)->cmd)
        free((*p)->cmd);
    if (((*p)->narg) > 0)
        free_CSV_arg(&((*p)->arg), (*p)->narg);
    free(*p);
}
/*free symbol_t node.*/
static void free_sym(void **node)
{
    symbol_t **p = (symbol_t **)node;
    if ((*p)->label)       /*if field is non NULL.*/
        free((*p)->label); /*free field.*/
    free(*p);
}
static void free_bin(void **node)
{
    free(*node);
}
/*the following function receives a address of a external_t node, then frees its allocated fields. and finally frees the node itself.*/
static void free_entry(void **node)
{
    external_t **p = (external_t **)node;
    if ((*p)->label)       /*if field is non NULL.*/
        free((*p)->label); /*free field.*/
    free(*p);
}
/*the following function gets a pointer to a list. the function frees it. 
free data node by its type. the function frees the address of a node by a void ptr, gets its type as enum.
the node is then freed by using the coresponding function with a pointer to it.*/
void list_free(list_t *list, int type)
{
    void (*fp_free_dt[])(void **) = {&free_dt, &free_sym, &free_bin, &free_entry};
    ptr p;                  /*pointer to store address of the node before going to the next.*/
    ptr *h = &(list->head); /*get the address of the head pointer*/
    while (*h)              /*iterate over the linked list*/
    {
        p = *h;          /*save address*/
        *h = (*h)->next; /*go to next node*/
        (*fp_free_dt[type])(&(p->data));
        free(p); /*free node p*/
    }
}
/*the following function prints the fields of a data_t object. receives data_t. return none.*/
static void fprint_dt(FILE *stream, void *node)
{
    data_t *p = (data_t *)node; /*cast by pointer, cast the generic void * pointer to data_t.*/
    puts("__________________________________");
    fprintf(stream, "at line = %d\n", (*p).line);
    if ((*p).label) /*if label exists*/
        fprintf(stream, "label = <%s>\n", (*p).label);
    if ((*p).cmd) /*if cmd exists*/
        fprintf(stream, "cmd  = <%s>\n", (*p).cmd);
    if (((*p).narg) > 0) /*if there are arguments.*/
    {
        int i = 0;
        fprintf(stream, "#arg = %d\n", (*p).narg);
        for (; i < (*p).narg; i++)
            fprintf(stream, "<%s>", *(p->arg + i));
        fprintf(stream, "\n");
    }
    fputs("__________________________________\n", stream);
}
/*print the binary represention of a given variable of size b_size .
input the address of the desired variable and its size in bits.*/
static void fprint_binary(FILE *stream, void *num, int b_size)
{
    uint64_t mask = 1;
    int i = 0;
    mask <<= b_size - 1;                /*move first bit to the end of the represention.*/
    for (; i < b_size; i++, mask >>= 1) /*print bits*/
    {
        fprintf(stream, "%s", *(uint64_t *)(num)&mask ? "1" : "0");
    }
    fprintf(stream, "\n");
}
/*intermediate function from the generic list printing to the generic binary print.*/
static void fprint_machine_word(FILE *stream, void *pnum)
{
    fprint_binary(stream, pnum, WORD_SIZE);
}
/*the following function prints the fields of a symbol_t object. receives data_t. return none.*/
static void fprint_symbol(FILE *stream, void *node)
{
    symbol_t *p = (symbol_t *)node; /*cast by pointer, cast the generic void * pointer to data_t.*/
    fputs("__________________________________\n", stream);
    if ((*p).label) /*if label exists*/
        fprintf(stream, "label=<%s>|", (*p).label);
    fprintf(stream, "address=<%d>|", (*p).address);
    fprintf(stream, "command=<%s>|", (*p).command == 1 ? "true" : "false");
    fprintf(stream, "external=<%s>|", (*p).external == 1 ? "true" : "false");
    fprintf(stream, "entry=<%s>\n", (*p).entry == 1 ? "true" : "false");
    fputs("__________________________________\n", stream);
}
/*prints to stream entry variables from given symbol list.*/
static void fprint_entry(FILE *stream, void *node)
{
    symbol_t *p = (symbol_t *)node; /*cast by pointer, cast the generic void * pointer to data_t.*/
    if ((*p).entry == TRUE)
        if ((*p).label) /*if label exists*/
            fprintf(stream, "%s\t%d\n", (*p).label, (*p).address);
}
/*the following function prints the fields of a data_t object. receives data_t. return none.*/
static void fprint_external(FILE *stream, void *node)
{
    external_t *p = (external_t *)node; /*cast by pointer, cast the generic void * pointer to data_t.*/
    if ((*p).label)                     /*if label exists*/

        fprintf(stream, "%s\t%d\n", (*p).label, (*p).address);
}
static void machine_word_b64print(FILE *stream, void *word)
{
    const char base64[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};
    unsigned int mask = 077;
    unsigned int MS6bits = *(int *)word;
    MS6bits >>= 6;                                                                  /*contains the 6 MSB of the given word.*/
    fprintf(stream, "%c%c\n", base64[MS6bits & mask], base64[*(int *)word & mask]); /*print to stream the 12bit represention in base64*/
}
/*print generic list to stream.
the type of the node is passed as an enum entry.
types supported:    - DATA_T = 0
                    - SYMBOL_T = 1
                    - INT_BIN_T = 2
                    - EXTERNAL_T = 3*/
void fprint_list(FILE *stream, list_t list, int type)
{
    void (*fp_prnt_dt[])(FILE *, void *) = {&fprint_dt, &fprint_symbol, &fprint_machine_word, &fprint_external, &fprint_entry, &machine_word_b64print}; /*array of printing functions, sorted by type.*/
    ptr h = list.head;                                                                                                                                  /*set pointer to head.*/
    while (h)                                                                                                                                           /*iterate thought all its nodes*/
    {
        (*fp_prnt_dt[type])(stream, (void *)h->data); /*activate printing by type. print node.*/
        h = h->next;                                  /*advance to the next node.*/
    }
}
/*the following function receives a pointer to a linked list and a generic data pointer.
the data is then pushed into the start of the list within a new node.
if successful, the address of the node is returned, else NULL is returned.*/
node_t *list_push(list_t *list, void *data)
{
    node_t *nnode = (node_t *)malloc(sizeof(node_t)); /*allocate new node.*/
    nnode->data = data;                               /*save data in node.*/
    if (!nnode)                                       /*check allocation.*/
    {
        printf("Allocation failed, line %d, file %s.\n", __LINE__, __FILE__);
        return NULL;
    }
    if (!list->head) /*if list is empty.*/
    {
        list->head = nnode; /*set new node as head.*/
        list->tail = nnode; /*set new node as tail.*/
        nnode->next = NULL; /*set node pointer to NULL, as its the only node in the list.*/
        nnode->prev = NULL;
        return nnode;
    }
    /*the list isnt empty.*/
    nnode->next = list->head; /*set current head as next of the new node.*/
    list->head->prev = nnode; /*set prev of the head to point to the new node.*/
    list->head = nnode;       /*set new node as head.*/
    nnode->prev = NULL;       /*terminate prev of the node.*/
    return nnode;
}
/*the following function receives a pointer to a linked list and a generic data pointer.
the data is then enqueued into the end of the list within a new node.
if successful, the address of the node is returned, else NULL is returned.*/
node_t *list_enqueue(list_t *list, void *data)
{
    node_t *nnode = (node_t *)malloc(sizeof(node_t)); /*allocate new node.*/
    nnode->data = data;
    if (!nnode)
    {
        printf("Allocation failed, line %d, file %s.\n", __LINE__, __FILE__);
        return NULL;
    }
    if (!list->tail) /*list is empty*/
    {
        list->head = nnode; /*same as push*/
        list->tail = nnode;
        nnode->next = NULL;
        nnode->prev = NULL;
        return nnode;
    }
    nnode->prev = list->tail; /*set current tail as prev of the new node.*/
    list->tail->next = nnode; /*set the new node as next of the current taul*/
    list->tail = nnode;       /*set node as the new tail.*/
    nnode->next = NULL;       /*terminate next of the new tail.*/
    return nnode;
}
/*this function receives a pointer to a list and a label to search in the given list.
if the label exists, its node address is return to the caller. otherwise NULL is returned.*/
void *search_label(list_t *sym_t, char *label)
{
    ptr h = sym_t->head; /*set pointer to the head of the list.*/
    if (sym_t->type != SYMBOL_T)
    {
        printf("[%s] wrong list type\n", "search_label"); /*__func__/__FUNCTION__ not supported in C90*/
        return NULL;
    }
    while (h)
    {
        symbol_t *p = (symbol_t *)h->data; /*set pointer to the data field of the list (cast by pointer).*/
        if (!(strcmp(p->label, label)))    /*if label found in symbol list*/
            return p;                      /*return node containing the label.*/
        h = h->next;                       /*procced to the next node.*/
    }
    return NULL; /*not found, return NULL.*/
}
/*chain given lists.*/
void chain_lists(list_t *dest, list_t *src)
{
    if (dest->type != src->type)
    {
        printf("[%s] list type mismatch \n", "chain_lists"); /*__func__/__FUNCTION__ not supported in C90*/
        return;
    }
    if (!(dest->head)) /*dest list is empty*/
    {
        dest->head = src->head;
        dest->tail = src->tail;
    }
    else if ((src->head)) /*src isnt empty*/
    {
        dest->tail->next = src->head;
        dest->tail = src->tail;
        src->head->prev = dest->head;
    }
    free(src);
}