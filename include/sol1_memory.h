//
// sol1_memory.h
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
#ifndef SOL1MEMORY_H
#define SOL1MEMORY_H

#include <stdio.h>
#include "sol1_registers.h"
#include "config.h"
#include "hw_tty.h"

class SOL1_MEMORY
{
public:
	SOL1_BYTE* mem_bios;

	SOL1_BYTE *mem_page_table0;
	SOL1_BYTE *mem_page_table1;

	SOL1_BYTE *main_memory;

	SOL1_BYTE *main_memory0;
	SOL1_BYTE *main_memory1;
	SOL1_BYTE *main_memory2;
	SOL1_BYTE *main_memory3;
	SOL1_BYTE *main_memory4;
	SOL1_BYTE *main_memory5;
	SOL1_BYTE *main_memory6;
	SOL1_BYTE *main_memory7;

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