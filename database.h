#ifndef DATABASE_H
#define DATABASE_H
#include <stdio.h>
#include <stdlib.h>
enum node_type
{
    DATA_T = 1
};
typedef struct data
{
    char *label;
    char *cmd;
    char *arg;
} data_t;
typedef struct node *ptr;
typedef struct node
{
    void *data;
    ptr next;
    ptr prev;
} node_t;
typedef struct list
{
    ptr head;
    ptr tail;
} list_t;
void list_print(list_t, int);
void list_free(list_t *, int);
void list_enqueue(list_t *, void *, int);
#endif