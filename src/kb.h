#ifndef KB_H
#define KB_H

#include "common.h"
#include "isr.h"
static void keyboard_handler(registers_t r);
void keyboard_install();

#endif