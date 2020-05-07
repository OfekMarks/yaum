#include <stddef.h>
#include <stdint.h>
#include "descriptors.h" 
#include "screen.h"

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif
 
/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif
 
 
void kmain(void) {
	/* initialize terminal interface */
	terminal_initialize();
	terminal_writestring("this is big siber os.\n");
 
	/* initialize the gdt */
	terminal_writestring("initializing gdt.\n");
	gdt_init();
	terminal_writestring("gdt initialized.");

	/* initialize the idt */
	terminal_writestring("initializing gdt.\n");
	idt_init();
	terminal_writestring("idt initialized.");
}
