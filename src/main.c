#include "monitor.h"
#include "descriptor_tables.h"
#include "timer.h"
#include "isr.h"
#include "kb.h"
#include "scheduler.h"

/*
static void isr3_callback(registers_t regs)
{
    monitor_write("This is interrupt service routine 3 \n");

}*/

void Task1(void *arg)
{
    task_t *task = (task_t *)arg;
    set_timer_periodic(40000); // set period in microseconds
    while (1)
    {
        monitor_write("Task 1\n");
        wait_timer_event(task);
    }
}

int main(struct multiboot *mboot_ptr)
{
    init_descriptor_tables();
    monitor_clear();
    monitor_write(" Welcome to OSx311 Kernel.\n");
    keyboard_install();

    // register_interrupt_handler(3, &isr3_callback);
    // asm volatile("int $0x3");
    // asm volatile("int $0x4");

    asm volatile("sti");
    init_timer(100);

    task_t *t1;
    t1 = create_task(1, 4, 40000, Task1, NULL); // create a task with id=1, priority=4, and period=40000
    add_task(t1);                               // add the task to the scheduler

    init_scheduler();
    asm volatile("int $0x22");
    return 0xDEADBABA;
}