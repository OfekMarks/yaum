#include "screen.h"
#include "../include/stdint.h"
#include "../include/stdlib.h"
#include "../include/string.h"
#include "tools.h"
#include <stddef.h>

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg);
static inline uint16_t vga_entry(unsigned char uc, uint8_t color);
static void terminal_putentryat(unsigned char c, uint8_t color, uint32_t x,
                                uint32_t y);
static void terminal_scroll_down(void);
static int can_delete(void);
static void terminal_writeint(int number, int base);
static void terminal_lock(void);

static uint32_t terminal_row;
static uint32_t terminal_column;
static uint8_t terminal_color;
static uint16_t *terminal_buffer;
static uint32_t terminal_row_lock;
static uint32_t terminal_column_lock;

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
  for (uint32_t y = 0; y < VGA_HEIGHT; y++) {
    for (uint32_t x = 0; x < VGA_WIDTH; x++) {
      const uint32_t index   = y * VGA_WIDTH + x;
      terminal_buffer[index] = vga_entry(' ', terminal_color);
    }
  }
}

static void terminal_putentryat(unsigned char c, uint8_t color, uint32_t x,
                                uint32_t y) {
  const uint32_t index   = y * VGA_WIDTH + x;
  terminal_buffer[index] = vga_entry(c, color);
}

void terminal_writechar(unsigned char c) {
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

void terminal_newline(void) {
  terminal_writechar('\n');
}

void terminal_write(const char *data, uint32_t size) {
  for (uint32_t i = 0; i < size; i++)
    terminal_writechar(data[i]);
}

void terminal_writestring(const char *data) {
  terminal_write(data, strlen(data));
  terminal_lock();
}

void terminal_writeint(int number, int base) {
  char string[11];
  itoa((unsigned)number, string, base);
  terminal_writestring(string);
  terminal_lock();
}

void terminal_writedec(int number) {
  terminal_writeint(number, 10);
}

void terminal_writehex(int number) {
  terminal_writechar('0');
  terminal_writechar('x');
  terminal_writeint(number, 16);
}

void terminal_writebin(uint8_t number) {
  char bin[9];
  binrep(bin, number);
  terminal_writechar('0');
  terminal_writechar('b');
  terminal_writestring(bin);
}

void terminal_clearscreen(void) {
  for (uint32_t y = 0; y < VGA_HEIGHT; y++) {
    for (uint32_t x = 0; x < VGA_WIDTH; x++) {
      const uint32_t index   = y * VGA_WIDTH + x;
      terminal_buffer[index] = vga_entry(' ', terminal_color);
    }
  }
  terminal_row    = 0;
  terminal_column = 0;
}

static void terminal_scroll_down(void) {
  for (uint32_t y = 0; y < VGA_HEIGHT; y++) {
    for (uint32_t x = 0; x < VGA_WIDTH; x++) {
      terminal_buffer[y * VGA_WIDTH + x] =
          terminal_buffer[(y + 1) * VGA_WIDTH + x];
    }
  }
  for (uint32_t x = 0; x < VGA_WIDTH; x++) {
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
  terminal_writechar(' ');

  if (--terminal_column == -1) {
    terminal_row--;
    terminal_column = VGA_WIDTH - 1;
  }
  return 1;
}

static void terminal_lock(void) {
  terminal_row_lock    = terminal_row;
  terminal_column_lock = terminal_column;
}

static int can_delete(void) {
  return (terminal_row_lock != terminal_row ||
          terminal_column_lock != terminal_column);
}

void terminal_printstatus(void) {
  uint32_t _terminal_row    = terminal_row;
  uint32_t _terminal_column = terminal_column;

  terminal_newline();
  terminal_newline();
  terminal_writeint(_terminal_row, 10);
  terminal_writechar(',');
  terminal_writeint(_terminal_column, 10);
  terminal_newline();
}
