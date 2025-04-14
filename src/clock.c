#include "common.h"
#include "monitor.h"
#include "clock.h"

#define CMOS_ADDRESS 0x70
#define CMOS_DATA 0x71

char last_second_str[3] = "60";

int get_update_in_progress_flag()
{
    outb(CMOS_ADDRESS, 0x0A);
    return (inb(CMOS_DATA) & 0x80);
}

unsigned char get_RTC_register(int reg)
{
    outb(CMOS_ADDRESS, reg);
    return inb(CMOS_DATA);
}

void read_rtc()
{
    unsigned char second;
    unsigned char minute;
    unsigned char hour;

    while (get_update_in_progress_flag())
    {
        // Make sure an update isn't in progress
    }
    second = get_RTC_register(0x00);
    minute = get_RTC_register(0x02);
    hour = get_RTC_register(0x04);

    // Convert BCD to binary values if necessary

    if (!(get_RTC_register(0x0B) & 0x04))
    {
        second = (second & 0x0F) + ((second / 16) * 10);
        minute = (minute & 0x0F) + ((minute / 16) * 10);
        hour = ((hour & 0x0F) + (((hour & 0x70) / 16) * 10)) | (hour & 0x80);
    }

    // At this point, the variables `hour`, `minute` and `second` hold the current time

    // Convert the time values to strings
    char second_str[3], minute_str[3], hour_str[3];
    sprintf(second_str, second);
    sprintf(minute_str, minute);
    sprintf(hour_str, hour);
    if (strcmp(second_str, last_second_str) != 0)
    {
        monitor_write_clock(hour_str, minute_str, second_str);
        strcpy(last_second_str, second_str);
    }
}

void set_system_clock(int hour, int minute, int second)
{
    // Set the update in progress flag
    outb(CMOS_ADDRESS, 0x0A);
    while (inb(CMOS_DATA) & 0x80)
    {
        // Wait for the update to finish
    }
    // Set the RTC registers with the given parameters
    outb(CMOS_ADDRESS, 0x00);
    outb(CMOS_DATA, second);
    outb(CMOS_ADDRESS, 0x02);
    outb(CMOS_DATA, minute);
    outb(CMOS_ADDRESS, 0x04);
    outb(CMOS_DATA, hour);
}