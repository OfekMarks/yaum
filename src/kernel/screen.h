#ifndef SCREEN_H
#define SCREEN_H

#include "../include/stdint.h"
#include <stddef.h>

void terminal_initialize(void);
void terminal_setcolor(uint8_t color);
void terminal_putchar(unsigned char c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);
void terminal_writestring_nonlock(const char* data);
void terminal_writeint(int number, int base);
void terminal_writeint_nonlock(int number, int base);
void terminal_clearscreen(void);
char terminal_deletechar(void);
void terminal_lock(void);
void terminal_printstatus(void);

/* Hardware text mode color constants. */
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

#endif
