#include "queue.h"
#include "scheduler.h"

#define INITIAL_CAPACITY 10

void enqueue(queue_t *q, task_t *task)
{
    // Add the task to the end of the heap
    q->heap[q->size] = task;
    q->size++;

    // Bubble up the task to its correct position
    int i = q->size - 1;
    while (i > 0 && q->heap[i]->priority > q->heap[(i - 1) / 2]->priority)
    {
        task_t *temp = q->heap[i];
        q->heap[i] = q->heap[(i - 1) / 2];
        q->heap[(i - 1) / 2] = temp;
        i = (i - 1) / 2;
    }
}

task_t *dequeue(queue_t *q)
{
    // Remove the highest-priority task from the front of the heap
    task_t *task = q->heap[0];
    q->heap[0] = q->heap[q->size - 1];
    q->size--;

    // Bubble down the new root task to its correct position
    int i = 0;
    while (2 * i + 1 < q->size)
    {
        int j = 2 * i + 1;
        if (j + 1 < q->size && q->heap[j]->priority < q->heap[j + 1]->priority)
        {
            j++;
        }
        if (q->heap[i]->priority >= q->heap[j]->priority)
        {
            break;
        }
        task_t *temp = q->heap[i];
        q->heap[i] = q->heap[j];
        q->heap[j] = temp;
        i = j;
    }

    return task;
}

task_t *peek(queue_t *q)
{
    // Return the highest-priority task without removing it
    return q->heap[0];
}

queue_t *queue_create()
{
    queue_t *q = (queue_t *)kmalloc(sizeof(queue_t));
    if (!q)
        return NULL; // Check if memory allocation was successful

    q->heap = (task_t **)kmalloc(sizeof(task_t *) * INITIAL_CAPACITY);
    if (!q->heap)
    {
        kfree(q); // Free the queue structure if heap allocation failed
        return NULL;
    }

    q->size = 0;
    q->capacity = INITIAL_CAPACITY;

    return q;
}