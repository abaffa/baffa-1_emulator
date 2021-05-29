#ifndef SOL1ROM_H
#define SOL1ROM_H

#include <stdio.h>
#include "config.h"

class SOL1_ROM
{

public:
	SOL1_BYTE *rom_desc;
	SOL1_BYTE **roms;

	SOL1_MWORD MAR; // memory address register

	int debug_mem_offset;

	SOL1_BYTE bkpt_opcode;
	SOL1_BYTE bkpt_cycle;

	void init(); //reset

	void display_current_cycles(SOL1_BYTE opcode, SOL1_BYTE cycle, SOL1_BYTE debug_desc_type);
	void display_current_cycles_desc(SOL1_BYTE opcode, SOL1_BYTE cycle);
	void debug_cycles();

private:
	void is_rom_in_bounds(SOL1_MWORD index);
	int load_rom(const char* filename, SOL1_BYTE* rom);
	void menu(SOL1_BYTE debug_desc_type);

};



#endif