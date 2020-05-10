global loader

extern kmain

MODULEALIGN equ  1<<0
MEMINFO     equ  1<<1
FLAGS       equ  MODULEALIGN | MEMINFO
MAGIC       equ    0x1BADB002
CHECKSUM    equ -(MAGIC + FLAGS)
STACKSIZE equ 0x4000

section .text

align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

loader:
	mov  esp, stack_top
	push eax
	push ebx

	call kmain

	cli
	.hang:
    		hlt
    		jmp  .hang

section .bss

align 16
stack_bottom:
        resb STACKSIZE
stack_top:
