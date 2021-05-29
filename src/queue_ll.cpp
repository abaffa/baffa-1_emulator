#include <stdio.h>
#include <stdlib.h>
#include "queue_ll.h"

struct queue_ll {
	Linked_List* begin;
	Linked_List* end;
};

Queue* queue_create(void) {
    Queue* f = (Queue*) malloc(sizeof(Queue));
    f->begin = NULL;
    f->end = NULL;
    return f;
}


void queue_insert(Queue* f, void* info) {
    
	Linked_List* n = ll_insert (NULL, info);
    
    
    if (f->end != NULL)
        f->end->next = n;
    else 
        f->begin = n;
    f->end = n;
}


void* queue_remove(Queue* f){

    void* info;
    
    if (queue_empty(f)) {
        return NULL; 
    } 
    
    info = f->begin->info;

    f->begin = ll_remove_first(f->begin);
    
    if (f->begin == NULL) 
        f->end = NULL;

    return info;
}


int queue_empty(Queue* f)
{
    return ll_empty(f->begin) && ll_empty(f->end);
}

void queue_free(Queue* f, void(*ffree)(void *))
{
	ll_free(f->begin, ffree);

    free(f);
}

void queue_print(Queue* f, void(*fprint)(void *))
{
    
    Linked_List* q = f->begin;
    while (q!=NULL) {
		fprint(q->info);
        q = q->next;
    }
}