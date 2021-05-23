#ifndef SOL1MEMORY_H
#define SOL1MEMORY_H

#include <stdio.h>
#include "sol1_registers.h"
#include "config.h"

struct sol1_memory
{
	SOL1_BYTE* memory; //[z80_MEMORY_SIZE];
	SOL1_REGISTER_8BIT *MARh; // memory address register
	SOL1_REGISTER_8BIT *MARl;
	SOL1_REGISTER_8BIT *MDRh; // memory data register
	SOL1_REGISTER_8BIT *MDRl;


	int debug_mem_offset;
	int debug_manual_offset;

	SOL1_BYTE memchip0[0x7FFF + 1];
	SOL1_BYTE memchip1[0x7FFF + 1];


	SOL1_BYTE memchip_test[0x7FFF + 1];

};

void sol1_memory_init(struct sol1_memory* memory); //reset
void sol1_memory_reset(struct sol1_memory* memory);
void sol1_memory_set(struct sol1_memory* memory, SOL1_MWORD index, SOL1_BYTE val); //set
unsigned char sol1_memory_get(struct sol1_memory* memory, SOL1_MWORD index); //enable
void sol1_memory_display(struct sol1_memory* memory);
#endif