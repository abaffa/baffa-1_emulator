#ifndef SOL1ROM_H
#define SOL1ROM_H

#include <stdio.h>
#include "config.h"

struct sol1_rom
{
	SOL1_BYTE *rom_desc;
	SOL1_BYTE roms [SOL1_ROM_NBR_ROMS][(SOL1_ROM_NBR_INSTRUCTIONS * SOL1_ROM_CYCLES_PER_INSTR)];

	SOL1_MWORD MAR; // memory address register
	
	int debug_mem_offset;

	SOL1_BYTE bkpt_opcode;
	SOL1_BYTE bkpt_cycle;
};


void sol1_rom_init(struct sol1_rom* rom); //reset
unsigned char sol1_rom_get(struct sol1_rom* rom, SOL1_MWORD index); //enable
void sol1_rom_display_current_cycles(struct sol1_rom* rom, SOL1_BYTE opcode, SOL1_BYTE cycle, SOL1_BYTE debug_desc_type);
void sol1_rom_display_current_cycles_desc(struct sol1_rom* rom, SOL1_BYTE opcode, SOL1_BYTE cycle);


void sol1_rom_debug_cycles(struct sol1_rom* rom);
#endif