#include "database.h"
/*the following function receives a address of a data_t node, then frees its allocated fields. and finally frees the node itself.*/
static void free_dt(void **node)
{
    data_t **p = (data_t **)node;
    if ((*p)->label)       /*if field is non NULL.*/
        free((*p)->label); /*free field.*/
    if ((*p)->cmd)
        free((*p)->cmd);
    if ((*p)->arg)
        free((*p)->arg);
    free(*p);
}
/*free data node by its type. the function recives the address of a node by a void ptr, and its type as enum.
the node is then freed by using the coresponding function with a pointer to it.*/
static void free_data(void **node, int type)
{
    void (*fp_free_dt[])(void **) = {&free_dt};
    (*fp_free_dt[type])(node);
}
/*the following function gets a pointer to a list. the function frees it.*/
void list_free(list_t *list, int type)
{
    ptr p;                  /*pointer to store address of the node before going to the next.*/
    ptr *h = &(list->head); /*get the address of the head pointer*/
    while (*h)              /*iterate over the linked list*/
    {
        p = *h;                      /*save address*/
        *h = (*h)->next;             /*go to next node*/
        free_data(&(p->data), type); /*free data from node p*/
        free(p);                     /*free node p*/
    }
}
/*the following function prints the fields of a data_t object. receives data_t. return none.*/
static void print_dt(void *node)
{
    data_t *p = (data_t *)node;
    puts("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
    if ((*p).label)
        printf("label =<%s>\n", (*p).label);
    if ((*p).cmd)
        printf("cmd  =<%s>\n", (*p).cmd);
    if ((*p).arg)
        printf("arg =<%s>\n", (*p).arg);
    puts("__________________________________");
}
/*the following function prints the given generic node, the type of the node is passed as an enum entry.*/
static void print_data(void *node, int type)
{
    void (*fp_prnt_dt[])(void *) = {&print_dt}; /*array of printing functions, sorted by type.*/
    (*fp_prnt_dt[type])(node);                  /*activate printing by type.*/
}
/*print list with given data type.*/
void list_print(list_t list, int type)
{
    ptr h = list.head; /*set pointer to head.*/
    switch (type)
    {
    case (DATA_T): /*deal with data_t data list.*/
        while (h)  /*iterate thought all its nodes*/
        {
            print_data((void *)h->data, DATA_T); /*print node.*/
            h = h->next;                         /*advance to the next node.*/
        }
        break;
    default:
        break;
    }
}
/*the following function receives a pointer to a linked list and a data_t obj.
the data is then pushed into the top of the list with a new node.*/
static void dt_push(list_t *list, data_t *data)
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
/*the following function receives a pointer to a linked list and a data_t obj.
the data is then enqueued into the end of the list with a new node.*/
static void dt_enqueue(list_t *list, data_t *data)
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
/*push to given list by data type.
arguments are list pointer , data pointer and the coresponding type (enum).
the function returns none.*/
void list_push(list_t *list, void *data, int type)
{
    switch (type)
    {
    case (DATA_T): /*deal with data_t type.*/
    {
        /*void (*fp_dt_push)(list_t *, data_t *) = &dt_push;*/
        data_t *p = (data_t *)data;
        /*(*fp_dt_push)(list, p);*/
        dt_push(list, p); /*push to list.*/
    }
    break;
    default:
        break;
    }
}
/*enqueue to given list by data type.
arguments are list pointer , data pointer and the coresponding type (enum).
the function returns none.*/
void list_enqueue(list_t *list, void *data, int type)
{
    switch (type)
    {
    case (DATA_T): /*deal with data_t type.*/
    {
        void (*fp_dt_enq)(list_t *, data_t *) = &dt_enqueue; /*pointer to enqueue function.*/
        data_t *p = (data_t *)data;                          /*cast by pointer to data_t **/
        (*fp_dt_enq)(list, p);                               /*enqueue to list.*/
    }
    break;
    default:
        break;
    }
}
