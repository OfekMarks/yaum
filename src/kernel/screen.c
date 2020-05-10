#include <stddef.h>
#include "../include/stdint.h"
#include "screen.h"
#include "../include/string.h"
#include "../include/stdlib.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_ADDRESS 0xB8000

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg);
static inline uint16_t vga_entry(unsigned char uc, uint8_t color);
static void terminal_scroll_down(void);

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
	return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
	return (uint16_t) uc | (uint16_t) color << 8;
}

void terminal_initialize(void) {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) VGA_ADDRESS;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}

}

static void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c) {

	if (terminal_row >= VGA_HEIGHT ||
			(terminal_row == (VGA_HEIGHT-1) && (terminal_column == VGA_WIDTH-1))) {
		terminal_scroll_down();
	}
	if (c == '\n'){
		terminal_row++;
		terminal_column = 0;
		return;
	}
	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT)
			terminal_row = 0;
	}
}

void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) {
	terminal_write(data, strlen(data));
}

void terminal_writeint(int number, int base) {
	char string[100];
	itoa((unsigned)number, string, base);
	terminal_writestring(string);
}

void terminal_clearscreen(void) {
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
	terminal_row = 0;
	terminal_column = 0;
}

static void terminal_scroll_down(void) {
	/*int i;
	terminal_clearscreen();
	terminal_writestring("abcd\n");
	terminal_writestring("efgh\n");
	// get first end of line (or the last character in the first row)
	for (i = 0; i < VGA_WIDTH; i++) {
			terminal_writeint(terminal_buffer[i] & 0xff, 16);
			terminal_putchar(' ');
			if (terminal_buffer[i] == vga_entry('\n', terminal_color)) {
				break;
			}
	}
	terminal_writeint(i, 10);
	//memcpy(terminal_buffer, terminal_buffer + i, VGA_WIDTH*VGA_HEIGHT - i);
	*/
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			terminal_buffer[y * VGA_WIDTH + x] = terminal_buffer[(y+1) * VGA_WIDTH + x];
		}
	}
	for (size_t x = 0; x < VGA_WIDTH; x++) {
		terminal_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
	}
	terminal_row = VGA_HEIGHT - 1;
	terminal_column = 0;
}
