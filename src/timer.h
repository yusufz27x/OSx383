#ifndef TIMER_H
#define TIMER_H

#include "isr.h"
#include "clock.h"
#include "scheduler.h"

void init_timer(u32int frequency);
void set_timer_periodic(u32int period);
void wait_timer_event(task_t *task);

#endif