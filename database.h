#ifndef DATABASE_H
#define DATABASE_H
#include <stdio.h>
#include <stdlib.h>
typedef struct node *ptr;
typedef struct node
{
    char *label;
    char *cmd;
    char *arg;
    ptr next;
    ptr prev;
} node_t;

typedef struct list
{
    ptr head;
    ptr tail;
} list_t;
#endif