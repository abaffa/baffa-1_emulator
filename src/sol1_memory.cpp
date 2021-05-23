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
		memory->memory[i] = 0x0;

	memory->MARl = sol1_register_8bit_create();
	memory->MARh = sol1_register_8bit_create();
	memory->MDRl = sol1_register_8bit_create();
	memory->MDRh = sol1_register_8bit_create();

	memory->debug_mem_offset = 0;
	memory->debug_manual_offset = 0x00;
}

void sol1_memory_reset(struct sol1_memory* memory)
{
	int address = 0;

	for (address = 0; address < SOL1_MEMORY_SIZE; address++)
		memory->memory[address] = 0x00;
}


void sol1_memory_set(struct sol1_memory* memory, SOL1_MWORD index, SOL1_BYTE val)
{
	sol1_is_memory_in_bounds(index);
	sol1_registers_set(memory->MARl, memory->MARh, index);

	memory->memory[index] = val;
}

unsigned char sol1_memory_get(struct sol1_memory* memory, SOL1_MWORD index)
{
	sol1_is_memory_in_bounds(index);

	SOL1_DWORD v = memory->memory[index];

	sol1_registers_set(memory->MARl, memory->MARh, index);
	sol1_registers_set(memory->MDRl, memory->MDRh, v);

	return v;
}

unsigned char sol1_memory_get(struct sol1_memory* memory)
{
	SOL1_MWORD index = sol1_registers_value(memory->MARl, memory->MARh);

	SOL1_DWORD v = memory->memory[index];

	sol1_registers_set(memory->MDRl, memory->MDRh, v);

	return v;
}


void sol1_memory_display(struct sol1_memory* memory)
{

	int i = 0, j = 0;
	SOL1_DWORD MAR = sol1_registers_value(memory->MARl, memory->MARh);

	if (memory->debug_manual_offset == 0x00) {
		if (0 + memory->debug_mem_offset > MAR || MAR >= 256 + memory->debug_mem_offset)
			memory->debug_mem_offset = (MAR / 0x10) * 0x10;
	}

	printf("\n        ");

	for (i = 0; i < 16; i++)
		printf("%02x ", i);

	printf("\n\n %04x ", memory->debug_mem_offset);

	for (i = 0 + memory->debug_mem_offset; i < 256 + memory->debug_mem_offset; i++) {
		if (i % 16 == 0)
			if (MAR == i)
				printf(" *");
			else
				printf("  ");
		if (MAR == i || MAR - 1 == i)
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
