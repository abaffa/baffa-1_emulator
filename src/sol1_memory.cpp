#include "sol1_memory.h"
#include <stdlib.h>
#include <assert.h>

static void sol1_is_memory_in_bounds(SOL1_MWORD index)
{
	assert(index >= 0 && index < SOL1_MEMORY_SIZE);
}

void sol1_memory_init(struct sol1_memory* memory)
{
	int i = 0;
	memory->memory = (SOL1_BYTE*)malloc(SOL1_MEMORY_SIZE * sizeof(SOL1_BYTE));

	for (i = 0; i < SOL1_MEMORY_SIZE; i++)
		memory->memory[i] = i % 0x100;// 0x0;

	memory->MAR = 0x0;
	memory->MDR = 0x0;
	
	
	memory->debug_mem_offset = 0;
}


void sol1_memory_set(struct sol1_memory* memory, SOL1_MWORD index, SOL1_BYTE val)
{
	sol1_is_memory_in_bounds(index);
	memory->MAR = index;
	memory->memory[memory->MAR] = val;
}

unsigned char sol1_memory_get(struct sol1_memory* memory, SOL1_MWORD index)
{
	sol1_is_memory_in_bounds(index);
	memory->MAR = index;
	memory->MDR = memory->memory[memory->MAR];
	return memory->MDR;
}


void sol1_display_memory(struct sol1_memory* memory)
{

	int i = 0, j = 0;

	if (0 + memory->debug_mem_offset > memory->MAR || memory->MAR >= 256 + memory->debug_mem_offset)
		memory->debug_mem_offset = (memory->MAR / 0x10) * 0x10;
	
	printf("\n        ");

	for (i = 0; i < 16; i++)
		printf("%02x ", i);
	
	printf("\n\n %04x ", memory->debug_mem_offset);

	for (i = 0 + memory->debug_mem_offset; i < 256 + memory->debug_mem_offset; i++) {
		if (i % 16 == 0)
			if (memory->MAR == i)
				printf(" *");
			else
				printf("  ");
		if (memory->MAR == i || memory->MAR - 1 == i)
			printf("%02x*", memory->memory[i]);
		else
			printf("%02x ", memory->memory[i]);

		if ((i + 1) % 16 == 0 && i <= 255 + memory->debug_mem_offset) {
			printf("  |");
			for (j = (i + 1) - 16; j < (i + 1); j++) {
				if (memory->memory[j] < 0x20)
					printf(".");
				else
					printf("%c", memory->memory[j]);
			}
			printf("|");

			if (i < 255 + memory->debug_mem_offset)
				printf("\n %04x ", i + 1);
			else
				printf("\n");

		}
	}
}
