#ifndef DEQUEUE_H
#define DEQUEUE_H

#include "linkedlist.h"

typedef struct queue_ll Queue;


Queue* queue_create (void);

void queue_insert (Queue* f, void* info);

void* queue_remove (Queue* f);

int queue_empty (Queue* f);

void queue_free (Queue* f, void (* ffree)(void *));

void queue_print (Queue* f, void (* fprint)(void *));

#endif