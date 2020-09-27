#include "screen.h"
#include "../include/stdint.h"
#include "../include/stdlib.h"
#include "../include/string.h"
#include <stddef.h>

#define VGA_WIDTH   80
#define VGA_HEIGHT  25
#define VGA_ADDRESS 0xB8000

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg);
static inline uint16_t vga_entry(unsigned char uc, uint8_t color);
static void terminal_putentryat(unsigned char c, uint8_t color, size_t x,
                                size_t y);
static void terminal_scroll_down(void);
static int can_delete(void);

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t *terminal_buffer;
static size_t terminal_row_lock;
static size_t terminal_column_lock;

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
  return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
  return (uint16_t)uc | (uint16_t)color << 8;
}

void terminal_initialize(void) {
  terminal_row    = 0;
  terminal_column = 0;
  terminal_lock();
  terminal_color  = vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK);
  terminal_buffer = (uint16_t *)VGA_ADDRESS;
  for (size_t y = 0; y < VGA_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      const size_t index     = y * VGA_WIDTH + x;
      terminal_buffer[index] = vga_entry(' ', terminal_color);
    }
  }
}

static void terminal_putentryat(unsigned char c, uint8_t color, size_t x,
                                size_t y) {
  const size_t index     = y * VGA_WIDTH + x;
  terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(unsigned char c) {
  if (terminal_row >= VGA_HEIGHT) {
    terminal_scroll_down();
  }
  if (c == '\n') {
    terminal_row++;
    terminal_column = 0;
    return;
  }
  terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
  if (++terminal_column == VGA_WIDTH) {
    terminal_column = 0;
    terminal_row++;
  }
}

void terminal_write(const char *data, size_t size) {
  for (size_t i = 0; i < size; i++)
    terminal_putchar(data[i]);
}

void terminal_writestring(const char *data) {
  terminal_write(data, strlen(data));
  terminal_lock();
}

void terminal_writestring_nonlock(const char *data) {
  terminal_write(data, strlen(data));
}

void terminal_writeint(int number, int base) {
  char string[100];
  itoa((unsigned)number, string, base);
  terminal_writestring(string);
  terminal_lock();
}

void terminal_writeint_nonlock(int number, int base) {
  char string[100];
  itoa((unsigned)number, string, base);
  terminal_writestring(string);
}

void terminal_clearscreen(void) {
  for (size_t y = 0; y < VGA_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      const size_t index     = y * VGA_WIDTH + x;
      terminal_buffer[index] = vga_entry(' ', terminal_color);
    }
  }
  terminal_row    = 0;
  terminal_column = 0;
}

static void terminal_scroll_down(void) {
  for (size_t y = 0; y < VGA_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      terminal_buffer[y * VGA_WIDTH + x] =
          terminal_buffer[(y + 1) * VGA_WIDTH + x];
    }
  }
  for (size_t x = 0; x < VGA_WIDTH; x++) {
    terminal_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] =
        vga_entry(' ', terminal_color);
  }
  terminal_row    = VGA_HEIGHT - 1;
  terminal_column = 0;

  terminal_row_lock--;
}

char terminal_deletechar(void) {
  if (!can_delete()) {
    return 0;
  }

  if (--terminal_column == -1) {
    terminal_row--;
    terminal_column = VGA_WIDTH - 1;
  }
  terminal_putchar(' ');

  if (--terminal_column == -1) {
    terminal_row--;
    terminal_column = VGA_WIDTH - 1;
  }
  return 1;
}

void terminal_lock(void) {
  terminal_row_lock    = terminal_row;
  terminal_column_lock = terminal_column;
}

static int can_delete(void) {
  return (terminal_row_lock != terminal_row ||
          terminal_column_lock != terminal_column);
}

void terminal_printstatus(void) {
  size_t _terminal_row    = terminal_row;
  size_t _terminal_column = terminal_column;

  terminal_putchar('\n');
  terminal_writeint(_terminal_row, 10);
  terminal_putchar(',');
  terminal_writeint(_terminal_column, 10);
  terminal_putchar('\n');
}
