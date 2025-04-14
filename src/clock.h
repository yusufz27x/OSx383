#ifndef CLOCK_H
#define CLOCK_H

#include "common.h"
#include "monitor.h"

int get_update_in_progress_flag();
unsigned char get_RTC_register(int reg);
void read_rtc();
void set_system_clock(int hour, int minute, int second);

#endif