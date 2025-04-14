#include "common.h"
#include "isr.h"
#include "monitor.h"
#include "kb.h"
#include "clock.h"

static char command[256];
static int command_index = 0;

void execute_command(char *command);

/* KBDUS means US Keyboard Layout. This is a scancode table
 *  used to layout a standard US keyboard. I have left some
 *  comments in to give you an idea of what key is what, even
 *  though I set it's array index to 0. You can change that to
 *  whatever you want using a macro, if you wish! */
unsigned char kbdus[128] =
	{
		0, 27, '1', '2', '3', '4', '5', '6', '7', '8',	  /* 9 */
		'9', '0', '-', '=', '\b',						  /* Backspace */
		'\t',											  /* Tab */
		'q', 'w', 'e', 'r',								  /* 19 */
		't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	  /* Enter key */
		0,												  /* 29   - Control */
		'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', /* 39 */
		'\'', '`', 0,									  /* Left shift */
		'\\', 'z', 'x', 'c', 'v', 'b', 'n',				  /* 49 */
		'm', ',', '.', '/', 0,							  /* Right shift */
		'*',
		0,	 /* Alt */
		' ', /* Space bar */
		0,	 /* Caps lock */
		0,	 /* 59 - F1 key ... > */
		0, 0, 0, 0, 0, 0, 0, 0,
		0, /* < ... F10 */
		0, /* 69 - Num lock*/
		0, /* Scroll Lock */
		0, /* Home key */
		0, /* Up Arrow */
		0, /* Page Up */
		'-',
		0, /* Left Arrow */
		0,
		0, /* Right Arrow */
		'+',
		0, /* 79 - End key*/
		0, /* Down Arrow */
		0, /* Page Down */
		0, /* Insert Key */
		0, /* Delete Key */
		0, 0, 0,
		0, /* F11 Key */
		0, /* F12 Key */
		0, /* All other keys are undefined */
};

/* Handles the keyboard interrupt */
static void keyboard_handler(registers_t r)
{
	unsigned char scancode;

	/* Read from the keyboard's data buffer */
	scancode = inb(0x60);

	/* If the top bit of the byte we read from the keyboard is
	 *  set, that means that a key has just been released */
	if (scancode & 0x80)
	{
		/* You can use this one to see if the user released the
		 *  shift, alt, or control keys... */
	}
	else
	{
		/* Here, a key was just pressed. Please note that if you
		 *  hold a key down, you will get repeated key press
		 *  interrupts. */

		/* Just to show you how this works, we simply translate
		 *  the keyboard scancode into an ASCII value, and then
		 *  display it to the screen. You can get creative and
		 *  use some flags to see if a shift is pressed and use a
		 *  different layout, or you can add another 128 entries
		 *  to the above layout to correspond to 'shift' being
		 *  held. If shift is held using the larger lookup table,
		 *  you would add 128 to the scancode when you look for it */

		char ascii = kbdus[scancode];
		monitor_put(ascii);

		if (ascii == '\n')
		{
			// Execute command
			execute_command(command);
			// Reset command
			command_index = 0;
			memset(command, 0, sizeof(command));
		}
		else
		{
			// Add ascii value to command
			command[command_index++] = ascii;
		}
	}
}

/* Installs the keyboard handler into IRQ1 */
void keyboard_install()
{
	register_interrupt_handler(IRQ1, keyboard_handler);
}

void execute_command(char *command)
{
	char *command_copy = strdup(command);

	char *args[10];
	int arg_count = 0;
	int start = 0, end = 0;

	// Get the command and the arguments
	while (command_copy[end] != '\0')
	{
		if (command_copy[end] == ' ' || command_copy[end + 1] == '\0')
		{
			if (command_copy[end] == ' ')
				command_copy[end] = '\0'; // replace space with null terminator

			args[arg_count++] = &command_copy[start];
			start = end + 1;
		}
		end++;
	}

	if (strcmp(args[0], "help") == 0)
	{
		// Display help information
		monitor_write("Supported commands are: help, reboot, settime, clear\n");
	}
	else if (strcmp(args[0], "reboot") == 0)
	{
		// Reboot the system
		reboot_system();
	}
	else if (strcmp(args[0], "settime") == 0)
	{
		// Check if the required number of arguments are present
		if (arg_count != 4)
		{
			monitor_write("settime usage:\"settime HH MM SS\"\n");
			return;
		}

		int hour = atoi(args[1]);
		int minute = atoi(args[2]);
		int second = atoi(args[3]);

		if (hour < 0 || hour > 23 || minute < 0 || minute > 59 || second < 0 || second > 59)
		{
			monitor_write("Invalid time for settime. Usage:\"settime HH MM SS\"\n");
			return;
		}

		// Set the system time
		set_system_clock(hour, minute, second);
	}
	else if (strcmp(args[0], "clear") == 0)
	{
		// Clear the screen
		monitor_clear();
		monitor_write(" OSx383> ");
	}
	else
	{
		monitor_write("Unknown command: ");
		monitor_write(args[0]);
		monitor_write("\n");
	}
}