#ifndef SOL1MEMORY_H
#define SOL1MEMORY_H

#include <stdio.h>
#include "sol1_registers.h"
#include "config.h"
#include "hw_tty.h"

class SOL1_MEMORY
{
public:
	SOL1_BYTE* memory;

	SOL1_BYTE *memchip0;
	SOL1_BYTE *memchip1;

	SOL1_BYTE *main_memory;

	/*
	SOL1_REGISTER_8BIT *MARh; // memory address register
	SOL1_REGISTER_8BIT *MARl;
	SOL1_REGISTER_8BIT *MDRh; // memory data register
	SOL1_REGISTER_8BIT *MDRl;
	*/

	int debug_mem_offset;
	int debug_manual_offset;

	   

	SOL1_MEMORY();
	void reset();

	int load_bios();

	void display(SOL1_REGISTERS& registers, HW_TTY& hw_tty);
	void display_test(SOL1_REGISTERS& registers, HW_TTY& hw_tty);
};
#endif