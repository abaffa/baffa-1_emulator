//
// sol1_rom.h
//
////// BEGIN LICENSE NOTICE//////
//
//Sol-1 HomebrewCPU Minicomputer System Emulator
//
//Copyright(C) 2021 Augusto Baffa, (sol-1.baffasoft.com.br)
//
//This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110 - 1301, USA.
//
////// END LICENSE NOTICE//////
//
#ifndef SOL1ROM_H
#define SOL1ROM_H

#include <stdio.h>
#include "config.h"
#include "hw_tty.h"

class SOL1_ROM
{

public:
	SOL1_BYTE *rom_desc;
	SOL1_BYTE **roms;

	SOL1_MWORD MAR; // memory address register

	int debug_mem_offset;

	SOL1_BYTE bkpt_opcode;
	SOL1_BYTE bkpt_cycle;

	void init(HW_TTY& hw_tty); //reset

	void display_current_cycles(SOL1_BYTE opcode, SOL1_BYTE cycle, SOL1_BYTE debug_desc_type, HW_TTY& hw_tty);
	void display_current_cycles_desc(SOL1_BYTE opcode, SOL1_BYTE cycle, HW_TTY& hw_tty);
	void debug_cycles(HW_TTY& hw_tty);

private:
	void is_rom_in_bounds(SOL1_MWORD index);
	int load_rom(const char* filename, SOL1_BYTE* rom, HW_TTY& hw_tty);
	void menu(SOL1_BYTE debug_desc_type, HW_TTY& hw_tty);

};



#endif