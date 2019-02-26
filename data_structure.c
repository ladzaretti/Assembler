#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"
/*array of strings of known data types.*/
const char ins_string[4][8] = {".data", ".string", ".entry", ".extern"};
/*array of strings with known commands names as defined is the assignment, to be compared using strcmp with input cmd from user.*/
const char cmd_string[16][5] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};
/*array of the system's registers.*/
const char registers[8][4] = {"@r0", "@r1", "@r2", "@r3", "@r4", "@r5", "@r6", "@r7"};
/*following function frees an dynamically allocated array of strings.
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
/*the following function gets a pointer to a list. the function frees it. 
free data node by its type. the function frees the address of a node by a void ptr, gets its type as enum.
the node is then freed by using the coresponding function with a pointer to it.*/
void list_free(list_t *list, int type)
{
    void (*fp_free_dt[])(void **) = {&free_dt, &free_sym};
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
/*the following function prints the fields of a symbol_t object. receives data_t. return none.*/
static void print_sym(void *node)
{
    symbol_t *p = (symbol_t *)node; /*cast by pointer, cast the generic void * pointer to data_t.*/
    puts("__________________________________");
    if ((*p).label) /*if label exists*/
        printf("label=<%s>|", (*p).label);
    printf("address=<%d>|", (*p).address);
    printf("command=<%s>|", (*p).command == 1 ? "true" : "false");
    printf("external=<%s>\n", (*p).external == 1 ? "true" : "false");
    puts("__________________________________");
}
/*the following function prints the given generic list, the type of the node is passed as an enum entry.
print list with given data type.*/
void list_print(list_t list, int type)
{
    void (*fp_prnt_dt[])(void *) = {&print_dt, &print_sym}; /*array of printing functions, sorted by type.*/
    ptr h = list.head;                                      /*set pointer to head.*/
    while (h)                                               /*iterate thought all its nodes*/
    {
        (*fp_prnt_dt[type])((void *)h->data); /*activate printing by type. print node.*/
        h = h->next;                          /*advance to the next node.*/
    }
}
/*the following function receives a pointer to a linked list and a generic data pointer.
the data is then pushed into the start of the list within a new node.*/
void list_push(list_t *list, void *data)
{
    node_t *nnode = (node_t *)malloc(sizeof(node_t)); /*allocate new node.*/
    nnode->data = data;                               /*save data in node.*/
    if (!nnode)                                       /*check allocation.*/
    {
        puts("allocation failed");
        return;
    }
    if (!list->head) /*if list is empty.*/
    {
        list->head = nnode; /*set new node as head.*/
        list->tail = nnode; /*set new node as tail.*/
        nnode->next = NULL; /*set node pointer to NULL, as its the only node in the list.*/
        nnode->prev = NULL;
        return;
    }
    /*the list isnt empty.*/
    nnode->next = list->head; /*set current head as next of the new node.*/
    list->head->prev = nnode; /*set prev of the head to point to the new node.*/
    list->head = nnode;       /*set new node as head.*/
    nnode->prev = NULL;       /*terminate prev of the node.*/
    return;
}
/*the following function receives a pointer to a linked list and a generic data pointer.
the data is then enqueued into the end of the list within a new node.*/
void list_enqueue(list_t *list, void *data)
{
    node_t *nnode = (node_t *)malloc(sizeof(node_t)); /*allocate new node.*/
    nnode->data = data;
    if (!nnode)
    {
        puts("allocation failed");
        return;
    }
    if (!list->tail) /*list is empty*/
    {
        list->head = nnode; /*same as push*/
        list->tail = nnode;
        nnode->next = NULL;
        nnode->prev = NULL;
        return;
    }
    nnode->prev = list->tail; /*set current tail as prev of the new node.*/
    list->tail->next = nnode; /*set the new node as next of the current taul*/
    list->tail = nnode;       /*set node as the new tail.*/
    nnode->next = NULL;       /*terminate next of the new tail.*/
    return;
}
/*the following function creates a symbol node with the given label.
input is a label string, the function then returns to the caller the address of the dynamically allocated node.
the node is initialized to 0 in all other fields.*/
symbol_t *create_symbol_node(char *label)
{
    symbol_t *node = (symbol_t *)calloc(1, sizeof(symbol_t)); /*allocate new data obj.*/
    if (node)
    {
        node->label = (char *)malloc(strlen(label) + 1); /*allocate space in symbol obj for the given label.*/
        if (!node->label)
        {
            printf("allocation failed.\n");
            return NULL;
        }
        strcpy(node->label, label); /*copy the label into the symbol obj.*/
        node->address = 0;          /*initialize node fields to 0.*/
        node->command = FALSE;
        node->external = FALSE;
    }
    else
    {
        printf("allocation failed.\n");
        return NULL;
    }
    return node;
}

/*this function receives a pointer to a list and a label to search in the given list.
if the label exists, its node address is return to the caller. otherwise NULL is returned.*/
symbol_t *search_label(list_t *sym_t, char *label)
{
    ptr h = sym_t->head; /*set pointer to the head of the list.*/
    while (h)
    {
        symbol_t *p = (symbol_t *)h->data; /*set pointer to the data field of the list (cast by pointer).*/
        if (!(strcmp(p->label, label)))    /*if label found in symbol list*/
        {
            err = TRUE; /*set error flag to TRUE.*/
            return p;   /*return node containing the label.*/
        }               /*did my best to minimize arrow code.*/

        h = h->next; /*procced to the next node.*/
    }
    return NULL; /*not found, return NULL.*/
}