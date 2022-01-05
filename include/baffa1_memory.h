//
// baffa1_memory.h
//
////// BEGIN LICENSE NOTICE//////
//
//Baffa-1 HomebrewCPU Minicomputer System Emulator
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
#ifndef BAFFA1MEMORY_H
#define BAFFA1MEMORY_H

#include <stdio.h>
#include "baffa1_registers.h"
#include "config.h"
#include "hw_tty.h"

class BAFFA1_MEMORY
{
public:
	BAFFA1_BYTE* mem_bios;

	BAFFA1_BYTE *mem_page_table0;
	BAFFA1_BYTE *mem_page_table1;

	BAFFA1_BYTE *low_memory;

	BAFFA1_BYTE *high_memory0;
	BAFFA1_BYTE *high_memory1;
	BAFFA1_BYTE *high_memory2;
	BAFFA1_BYTE *high_memory3;
	BAFFA1_BYTE *high_memory4;
	BAFFA1_BYTE *high_memory5;
	BAFFA1_BYTE *high_memory6;
	BAFFA1_BYTE *high_memory7;

	/*
	BAFFA1_REGISTER_8BIT *MARh; // memory address register
	BAFFA1_REGISTER_8BIT *MARl;
	BAFFA1_REGISTER_8BIT *MDRh; // memory data register
	BAFFA1_REGISTER_8BIT *MDRl;
	*/

	int debug_mem_offset;
	int debug_manual_offset;

	   

	BAFFA1_MEMORY();
	void reset();

	int load_bios();

	void displayMainMemory(BAFFA1_REGISTERS& registers, HW_TTY& hw_tty);
	void displayBiosMemory(BAFFA1_REGISTERS& registers, HW_TTY& hw_tty);
	void displayLowMemory(BAFFA1_REGISTERS& registers, HW_TTY& hw_tty);
	
	void display_test(BAFFA1_REGISTERS& registers, HW_TTY& hw_tty);

private:
	void displayMemory(BAFFA1_BYTE* mem_bytes, int offset, BAFFA1_MWORD MAR, HW_TTY &hw_tty);
	
};
#endif