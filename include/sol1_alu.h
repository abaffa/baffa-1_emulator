//
// sol1_alu.h
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
#ifndef SOL1ALU_H
#define SOL1ALU_H

#include "sol1_alu_4bit.h"
#include "sol1_controller_bus.h"
#include "sol1_alu_bus.h"
#include "config.h"
#include "hw_tty.h"


struct sol1_alu
{
	SOL1_BYTE _A;
	SOL1_BYTE _B;
	SOL1_BYTE _C;

	SOL1_BYTE A;
	SOL1_BYTE B;

	SOL1_BYTE C;

	SOL1_BYTE CIN; //carry in

	SOL1_BYTE COUT; //carry out

	SOL1_BYTE EQ; //EQ
	SOL1_BYTE F; //Larger, equal, zero, carry out
};


void sol1_alu_init(struct sol1_alu *alu);
void sol1_alu_display_registers(struct sol1_alu *alu, struct sol1_controller_bus *controller_bus, struct sol1_alu_bus *alu_bus, HW_TTY& hw_tty);


SOL1_BYTE ALU_EXEC(sol1_alu *alu, struct sol1_controller_bus *controller_bus, struct sol1_alu_bus *alu_bus,
	SOL1_BYTE u_cf, SOL1_BYTE msw_cf, SOL1_CONFIG& config, HW_TTY& hw_tty);


#endif