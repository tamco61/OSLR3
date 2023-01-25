#include "queue.h"

void create(queue **q)
{
    *q = malloc(sizeof(queue));
    (*q)->size = 0;
    (*q)->first = (*q)->last = NULL;
}

bool isEmpty(const queue *q)
{
    return q->size == 0;
}

void pop(queue *q)
{
    if (isEmpty(q)) {
        return;
    }

    if (q->size == 1) {
        free(q->first);
        q->first = q->last = NULL;
    }
    else {
        q->first = q->first->next;
        q->first->prev->next = NULL;
        free(q->first->prev);
        q->first->prev = NULL;
    }
    q->size--;
}

void push(queue *q, pthread_t *pt)
{
    if (isEmpty(q)) {
        q->first = malloc(sizeof(list));
        q->first->next = NULL;
        q->first->prev = NULL;
        q->last = q->first;
        q->first->thread = pt;
    }
    else {
        q->last->next = malloc(sizeof(list));
        q->last->next->prev = q->last;
        q->last->next->next = NULL;
        q->last->next->thread = pt;
        q->last = q->last->next;
    }
    q->size++;
}

size_t sizes(const queue *q)
{
    return q->size;
}

void deletes(queue *q)
{
    if (q == NULL) {
        return;
    }
    while (!isEmpty(q)) {
        pop(q);
    }
}