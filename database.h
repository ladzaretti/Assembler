#ifndef DATABASE_H
#define DATABASE_H
#include <stdio.h>
#include <stdlib.h>
/*list of differant linked list data types.*/
enum node_type
{
    DATA_T = 0
};
/*struct to store parsed line*/
typedef struct data
{
    char *label;
    char *cmd;
    char **arg;
    int narg;
} data_t;
/*generic double linked list, data fiels is changable.*/
typedef struct node *ptr;
typedef struct node
{
    void *data;
    ptr next;
    ptr prev;
} node_t;
/*generic list struct, contains head and tail pointers.*/
typedef struct list
{
    ptr head;
    ptr tail;
} list_t;

void list_print(list_t, int);
void list_free(list_t *, int);
void list_push(list_t *, void *, int);
void list_enqueue(list_t *, void *, int);
#endif