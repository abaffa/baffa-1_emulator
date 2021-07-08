//
// sol1_cpu.h
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
#ifndef SOL1CPU_H
#define SOL1CPU_H

#include "config.h"
#include "sol1_memory.h"
#include "sol1_microcode.h"
#include "sol1_registers.h"
#include "sol1_alu.h"
#include "hw_tty.h"
#include <stddef.h>

class SOL1_CPU
{

public:

	SOL1_MEMORY memory;
	SOL1_REGISTERS registers;
	SOL1_MICROCODE microcode;

	struct sol1_alu alu;

	char last_op_desc[512];

	SOL1_MWORD BKPT; //breakpoint (FFFF = disable)

	SOL1_CONFIG config;


	int display_reg_load;


	void init(HW_TTY& hw_tty);

	void reset();
	void memory_display(HW_TTY& hw_tty);


	void display_registers(HW_TTY& hw_tty);
	void display_registers_lite(HW_TTY& hw_tty);


	SOL1_BYTE *get_current_memory();

	int get_current_memory_size();




};
#endif