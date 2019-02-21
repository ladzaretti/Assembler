#include "database.h"
static void print_dt(data_t node)
{
    if (node.label)
        printf("label =%s\n", node.label);
    if (node.cmd)
        printf("cmd  =%s\n", node.cmd);
    if (node.arg)
        printf("arg =%s\n", node.arg);
}
static void free_dt(data_t **node)
{
    if ((*node)->label)
        free((*node)->label);
    if ((*node)->cmd)
        free((*node)->cmd);
    if ((*node)->arg)
        free((*node)->arg);
    free(*node);
}

void print_data(void *node, int type)
{

    switch (type)
    {
    case (DATA_T):
    {
        void (*fp_prnt_dt)(data_t) = &print_dt;
        data_t *p = (data_t *)node;
        (*fp_prnt_dt)(*p);
    }
    break;
    default:
        break;
    }
}
void free_data(void **node, int type)
{

    switch (type)
    {
    case (DATA_T):
    {
        void (*fp_free_dt)(data_t **) = &free_dt;
        data_t **p = (data_t **)node;
        (*fp_free_dt)(p);
    }
    break;
    default:
        break;
    }
}
static void dt_enqueue(list_t list, data_t *data)
{
    node_t *nnode = (node_t *)malloc(sizeof(node_t));
    nnode->data = data;
    if (!nnode)
    {
        puts("allocation failed");
        return;
    }
    if (!list.head)
    {
        list.head = nnode;
        list.tail = nnode;
        return;
    }
    list.head->next = nnode;
    nnode->next = NULL;
    return;
}
void list_enqueue(list_t list, void *data, int type)
{

    switch (type)
    {
    case (DATA_T):
    {
        void (*fp_dt_enq)(list_t, data_t *) = &dt_enqueue;
        data_t *p = (data_t *)data;
        (*fp_dt_enq)(list,p);
    }
    break;
    default:
        break;
    }
}
