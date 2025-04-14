#include "common.h"

char heap[HEAP_SIZE];
int heap_index = 0;

// Write byte to port
void outb(u16int port, u8int value)
{
    asm volatile("outb %1, %0" : : "dN"(port), "a"(value));
}

u8int inb(u16int port)
{
    u8int ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "dN"(port));
    return ret;
}

// Read byte from port
u16int inw(u16int port)
{
    u16int ret;
    asm volatile("inw %1, %0" : "=a"(ret) : "dN"(port));
    return ret;
}

// Copy len bytes from src to dest
void memcpy(u8int *dest, const u8int *src, u32int len)
{
    const u8int *sp = (const u8int *)src;
    u8int *dp = (u8int *)dest;
    for (; len != 0; len--)
        *dp++ = *sp++;
}

// Fill dest to dest+len with val
void memset(u8int *dest, u8int val, u32int len)
{
    u8int *temp = (u8int *)dest;
    for (; len != 0; len--)
        *temp++ = val;
}

// Compare two strings
int strcmp(char *str1, char *str2)
{
    int i = 0;
    int failed = 0;
    while (str1[i] != '\0' && str2[i] != '\0')
    {
        if (str1[i] != str2[i])
        {
            failed = 1;
            break;
        }
        i++;
    }
    if ((str1[i] == '\0' && str2[i] != '\0') || (str1[i] != '\0' && str2[i] == '\0'))
        failed = 1;

    return failed;
}

// Copy src to dest
char *strcpy(char *dest, const char *src)
{
    do
    {
        *dest++ = *src++;
    } while (*src != 0);
}

// Concatenate dest with src
char *strcat(char *dest, const char *src)
{
    while (*dest != 0)
    {
        *dest = *dest++;
    }

    do
    {
        *dest++ = *src++;
    } while (*src != 0);
    return dest;
}

void sprintf(char *str, int num)
{
    int tens = num / 10;
    int ones = num % 10;

    str[0] = '0' + tens;
    str[1] = '0' + ones;
    str[2] = '\0';
}

void reboot_system()
{
    u8int temp;

    asm volatile("cli"); // disable all interrupts

    // 8042 keyboard controller: CPU reset line
    for (temp = 0x02; temp & 0x02; temp = inb(0x64))
    {
        asm volatile("nop");
    }

    outb(0x64, 0xFE); // pulse CPU reset line

    asm volatile("sti"); // enable all interrupts

    // if that didn't work, halt the system
    while (1)
    {
        asm volatile("hlt");
    }
}

int atoi(const char *str)
{
    int result = 0;
    int sign = 1;

    // Check for optional sign
    if (*str == '-')
    {
        sign = -1;
        str++;
    }
    else if (*str == '+')
    {
        str++;
    }

    // Conversion of each digit
    while (*str)
    {
        if (*str < '0' || *str > '9')
        {
            break; // Non-digit character, stop conversion
        }
        result = result * 10 + (*str - '0');
        str++;
    }

    return sign * result;
}

char *strdup(const char *src)
{
    static char buffer[MAX_STRING_SIZE];
    int i;

    // Copy the original string into the buffer
    for (i = 0; i < MAX_STRING_SIZE - 1 && src[i] != '\0'; i++)
    {
        buffer[i] = src[i];
    }

    // Null-terminate the string in the buffer
    buffer[i] = '\0';

    return buffer;
}

void *kmalloc(u32int size)
{
    if (heap_index + size > HEAP_SIZE)
    {
        // Out of memory!
        return NULL;
    }

    void *ptr = &heap[heap_index];
    heap_index += size;

    // Zero out the memory
    memset(ptr, 0, size);

    return ptr;
}

void kfree(void *ptr)
{
    // No need to implement this for now
}