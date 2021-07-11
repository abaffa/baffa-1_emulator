//
// sol1_bus.h
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
#ifndef SOL1BUS_H
#define SOL1BUS_H
#include "config.h"
#include "sol1_registers.h"
#include "sol1_alu_bus.h"
#include "hw_tty.h"

class SOL1_BUS {
	
public:
	SOL1_BYTE data_bus;

	SOL1_BYTE k_bus; // input pra alu k -> y
	SOL1_BYTE w_bus; // input pra alu w -> x
	
	struct sol1_alu_bus alu_bus;

	/////
	SOL1_BYTE bus_tristate(SOL1_REGISTERS& sol1_registers);
	SOL1_BYTE bus_rd(SOL1_REGISTERS& sol1_registers, SOL1_BYTE rd);
	SOL1_BYTE bus_wr(SOL1_REGISTERS& sol1_registers, SOL1_BYTE wr);
	/////

	void init();
	void reset();

	SOL1_BYTE k_bus_refresh(SOL1_REGISTERS& sol1_registers, SOL1_BYTE alu_b_src);

	SOL1_BYTE w_bus_refresh(SOL1_REGISTERS& sol1_registers, SOL1_BYTE panel_regsel, SOL1_BYTE alu_a_src, SOL1_BYTE display_reg_load,
		SOL1_BYTE int_vector, SOL1_BYTE int_masks, SOL1_BYTE int_status, FILE *fa, SOL1_BYTE DEBUG_RDREG, HW_TTY& hw_tty);

	SOL1_BYTE x_bus_refresh(SOL1_REGISTERS& sol1_registers,
		SOL1_BYTE alu_a_src, SOL1_BYTE w_bus);
};
#endif