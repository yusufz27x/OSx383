#include "scheduler.h"
#include "timer.h"
#include "isr.h"
#include "queue.h" // You'll need to implement a priority queue
#include "monitor.h"

#define QUANTUM_TIME 10

static task_t *current_task = 0;
static queue_t *task_queue; // Priority queue of tasks

void scheduler(registers_t regs)
{
    if (current_task && --current_task->quantum > 0)
    {
        return; // If the current task still has quantum time, continue running it
    }

    if (current_task)
    {
        current_task->quantum = QUANTUM_TIME; // Reset the quantum
        enqueue(task_queue, current_task);    // If the current task has expired, re-add it to the queue
    }

    task_t *new_task = dequeue(task_queue); // Dequeue the next task from the queue

    if (new_task)
    {
        switch_task(new_task); // Switch to the new task's context
    }
}

void init_scheduler()
{
    task_queue = queue_create();
    // Initialize the scheduler
    register_interrupt_handler(IRQ2, &scheduler);
    monitor_write("Initializing scheduler\n");
}

void switch_task(task_t *new_task)
{
    asm volatile("mov %%esp, %0" : "=r"(current_task->esp));
    asm volatile("mov %%ebp, %0" : "=r"(current_task->ebp));
    current_task = new_task;
    asm volatile("mov %0, %%esp" ::"r"(current_task->esp));
    asm volatile("mov %0, %%ebp" ::"r"(current_task->ebp));

    // Call the task function
    if (current_task->task_func)
    {
        current_task->task_func(current_task->task_arg);
    }
}

task_t *create_task(int id, int priority, int period, void (*task_func)(void *), void *task_arg)
{
    task_t *new_task = (task_t *)kmalloc(sizeof(task_t));
    new_task->id = id;
    new_task->priority = priority;
    new_task->quantum = period ? period : QUANTUM_TIME;
    new_task->esp = new_task->ebp = 0; // Initialize stack pointers
    new_task->regs = (registers_t){0}; // Initialize regs
    new_task->task_func = task_func;   // Set the task function
    new_task->task_arg = task_arg;     // Set the task function argument
    return new_task;
}

void add_task(task_t *task)
{
    enqueue(task_queue, task);
}

void update_task_periods(u32int tick)
{
    for (int i = 0; i < task_queue->size; i++)
    {
        task_t *task = task_queue->heap[i];
        if (tick % task->quantum == 0)
        {
            enqueue(task_queue, task);
        }
    }
}