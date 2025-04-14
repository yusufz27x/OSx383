#ifndef QUEUE_H
#define QUEUE_H

#include "scheduler.h"

typedef struct
{
    task_t **heap;
    int size;
    int capacity;
} queue_t;

void enqueue(queue_t *q, task_t *task);
task_t *dequeue(queue_t *q);
task_t *peek(queue_t *q);
queue_t *queue_create();

#endif