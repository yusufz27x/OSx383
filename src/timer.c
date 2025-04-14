#include "timer.h"
#include "isr.h"
#include "clock.h"
#include "scheduler.h"

u32int tick = 0;

static void timer_callback(registers_t regs)
{
    tick++;
    if (tick % 18 == 0)
    {
        // Seconds passed loop
        /* monitor_write("Seconds passed: ");
        monitor_write_dec(tick / 18);
        monitor_write("\n"); */
        read_rtc();
    }
    update_task_periods(tick);
}

void init_timer(u32int frequency)
{
    register_interrupt_handler(IRQ0, &timer_callback);

    u32int divisor = 1193180 / frequency;

    outb(0x43, 0x36);

    u8int l = (u8int)(divisor & 0xFF);
    u8int h = (u8int)((divisor >> 8) & 0xFF);

    outb(0x40, l);
    outb(0x40, h);
}

void set_timer_periodic(u32int period)
{
    init_timer(period);
}

void wait_timer_event(task_t *task)
{
    task->quantum = 0;     // Expire the current task's quantum
    scheduler(task->regs); // Call the scheduler to switch tasks
}