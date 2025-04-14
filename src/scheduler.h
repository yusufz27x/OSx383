#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "isr.h"
#include "clock.h"
#include "common.h"

typedef struct task
{
    int id;          // Task ID
    int priority;    // Task priority
    int quantum;     // Task quantum
    u32int esp, ebp; // Stack and base pointers
    u32int eip;      // Instruction pointer
    registers_t regs;
    void (*task_func)(void *); // Task function
    void *task_arg;            // Argument to the task function
} task_t;

void scheduler(registers_t regs);
void init_scheduler();
void switch_task(task_t *new_task);
void update_task_periods(u32int tick);
task_t *create_task(int id, int priority, int period, void (*task_func)(void *), void *task_arg);
void add_task(task_t *task);

#endif