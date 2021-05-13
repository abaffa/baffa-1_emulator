#ifndef SOL1MEMORY_H
#define SOL1MEMORY_H

#include <stdio.h>
#include "config.h"

struct sol1_memory
{
	SOL1_BYTE* memory; //[z80_MEMORY_SIZE];
	SOL1_MWORD MAR; // memory address register
	SOL1_DWORD MDR; // memory data register


	int debug_mem_offset;
};

void sol1_memory_init(struct sol1_memory* memory); //reset
void sol1_memory_set(struct sol1_memory* memory, SOL1_MWORD index, SOL1_BYTE val); //set
unsigned char sol1_memory_get(struct sol1_memory* memory, SOL1_MWORD index); //enable
void sol1_display_memory(struct sol1_memory* memory);
#endif