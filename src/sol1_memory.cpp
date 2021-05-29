#include "sol1_memory.h"
#include <stdlib.h>
#include <assert.h>


SOL1_MEMORY::SOL1_MEMORY()
{

	int i = 0;
	this->memory = (SOL1_BYTE*)malloc(SOL1_BIOS_MEMORY_SIZE * sizeof(SOL1_BYTE));
	
	this->memchip0 = (SOL1_BYTE*)malloc(SOL1_PAGING_MEMORY_SIZE * sizeof(SOL1_BYTE));
	this->memchip1 = (SOL1_BYTE*)malloc(SOL1_PAGING_MEMORY_SIZE * sizeof(SOL1_BYTE));
	
	this->main_memory = (SOL1_BYTE*)malloc(SOL1_MAIN_MEMORY_SIZE * sizeof(SOL1_BYTE));

	this->debug_mem_offset = 0;
	this->debug_manual_offset = 0x00;

	this->reset();
}

void SOL1_MEMORY::reset()
{
	int address = 0;

	for (address = 0; address < SOL1_BIOS_MEMORY_SIZE; address++){
		this->memory[address] = 0x00;
	}
	for (address = 0; address < SOL1_PAGING_MEMORY_SIZE; address++) {
		this->memchip0[address] = 0x00;
		this->memchip1[address] = 0x00;
	}
	for (address = 0; address < SOL1_MAIN_MEMORY_SIZE; address++) {
		this->main_memory[address] = 0x00;

	}

}


void SOL1_MEMORY::display(SOL1_REGISTERS& registers)
{
	
	int i = 0, j = 0;
	SOL1_DWORD MAR = SOL1_REGISTERS::value(registers.MARl, registers.MARh);

	if (this->debug_manual_offset == 0x00) {
		if (0 + this->debug_mem_offset > MAR || MAR >= 256 + this->debug_mem_offset)
			this->debug_mem_offset = (MAR / 0x10) * 0x10;
	}

	printf("\n        ");

	for (i = 0; i < 16; i++)
		printf("%02x ", i);

	printf("\n\n %04x ", this->debug_mem_offset);

	for (i = 0 + this->debug_mem_offset; i < 256 + this->debug_mem_offset; i++) {
		if (i % 16 == 0)
			if (MAR == i)
				printf(" *");
			else
				printf("  ");
		if (MAR == i || MAR - 1 == i)
			printf("%02x*", this->memory[i]);
		else
			printf("%02x ", this->memory[i]);

		if ((i + 1) % 16 == 0 && i <= 255 + this->debug_mem_offset) {
			printf("  |");
			for (j = (i + 1) - 16; j < (i + 1); j++) {
				if (this->memory[j] < 0x20)
					printf(".");
				else
					printf("%c", this->memory[j]);
			}
			printf("|");

			if (i < 255 + this->debug_mem_offset)
				printf("\n %04x ", i + 1);
			else
				printf("\n");

		}
	}
}




void SOL1_MEMORY::display_test(SOL1_REGISTERS&  registers)
{

	int i = 0, j = 0;
	SOL1_DWORD MAR = SOL1_REGISTERS::value(registers.MARl, registers.MARh);

	if (this->debug_manual_offset == 0x00) {
		if (0 + this->debug_mem_offset > MAR || MAR >= 256 + this->debug_mem_offset)
			this->debug_mem_offset = (MAR / 0x10) * 0x10;
	}

	printf("\n        ");

	for (i = 0; i < 16; i++)
		printf("%02x ", i);

	printf("\n\n %04x ", this->debug_mem_offset);

	for (i = 0 + this->debug_mem_offset; i < 256 + this->debug_mem_offset; i++) {
		if (i % 16 == 0)
			if (MAR == i)
				printf(" *");
			else
				printf("  ");
		if (MAR == i || MAR - 1 == i)
			printf("%02x*", this->main_memory[i]);
		else
			printf("%02x ", this->main_memory[i]);

		if ((i + 1) % 16 == 0 && i <= 255 + this->debug_mem_offset) {
			printf("  |");
			for (j = (i + 1) - 16; j < (i + 1); j++) {
				if (this->main_memory[j] < 0x20)
					printf(".");
				else
					printf("%c", this->main_memory[j]);
			}
			printf("|");

			if (i < 255 + this->debug_mem_offset)
				printf("\n %04x ", i + 1);
			else
				printf("\n");

		}
	}
}
