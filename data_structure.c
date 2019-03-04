#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "database.h"
#define WORD_SIZE 12
/*initilize linked list with NULL.
input - address of a list.*/
void initilize_list(list_t *list)
{
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
static void print_dt(void *node)
{
    data_t *p = (data_t *)node; /*cast by pointer, cast the generic void * pointer to data_t.*/
    puts("__________________________________");
    if ((*p).label) /*if label exists*/
        printf("label = <%s>\n", (*p).label);
    if ((*p).cmd) /*if cmd exists*/
        printf("cmd  = <%s>\n", (*p).cmd);
    if (((*p).narg) > 0) /*if there are arguments.*/
    {
        int i = 0;
        printf("#arg = %d\n", (*p).narg);
        for (; i < (*p).narg; i++)
            printf("<%s>", *(p->arg + i));
        printf("\n");
    }
    puts("__________________________________");
}
/*print the binary represention of a given variable of size b_size .
input the address of the desired variable and its size in bits.*/
void binary_print(void *num, int b_size)
{
    uint64_t mask = 1;
    int i = 0;
    mask <<= b_size - 1;                /*move first bit to the end of the represention.*/
    for (; i < b_size; i++, mask >>= 1) /*print bits*/
    {
        printf("%s", *(uint64_t *)(num)&mask ? "1" : "0");
    }
    printf("\n");
}
/*intermediate function from the generic list printing to the generic binary print.*/
static void print_machine_word(void *pnum)
{
    binary_print(pnum, WORD_SIZE);
}
/*the following function prints the fields of a symbol_t object. receives data_t. return none.*/
static void print_sym(void *node)
{
    symbol_t *p = (symbol_t *)node; /*cast by pointer, cast the generic void * pointer to data_t.*/
    puts("__________________________________");
    if ((*p).label) /*if label exists*/
        printf("label=<%s>|", (*p).label);
    printf("address=<%d>|", (*p).address);
    printf("command=<%s>|", (*p).command == 1 ? "true" : "false");
    printf("external=<%s>|", (*p).external == 1 ? "true" : "false");
    printf("entry=<%s>\n", (*p).entry == 1 ? "true" : "false");
    puts("__________________________________");
}
/*the following function prints the fields of a data_t object. receives data_t. return none.*/
static void print_entry(void *node)
{
    external_t *p = (external_t *)node; /*cast by pointer, cast the generic void * pointer to data_t.*/
    puts("__________________________________");
    if ((*p).label) /*if label exists*/
        printf("label = <%s>\n", (*p).label);
    printf("address  = <%d>\n", (*p).address);
    puts("__________________________________");
}
/*the following function prints the given generic list, the type of the node is passed as an enum entry.
print list with given data type.*/
void list_print(list_t list, int type)
{
    void (*fp_prnt_dt[])(void *) = {&print_dt, &print_sym, &print_machine_word, &print_entry}; /*array of printing functions, sorted by type.*/
    ptr h = list.head;                                                                         /*set pointer to head.*/
    while (h)                                                                                  /*iterate thought all its nodes*/
    {
        (*fp_prnt_dt[type])((void *)h->data); /*activate printing by type. print node.*/
        h = h->next;                          /*advance to the next node.*/
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