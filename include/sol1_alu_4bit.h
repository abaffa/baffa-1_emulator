//
// sol1_alu_4bit.h
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
#ifndef SOL1ALU4BIT_H
#define SOL1ALU4BIT_H

#include "config.h"

struct sol1_alu_4bit
{
	SOL1_BYTE alu_output;
	SOL1_BYTE COUT; //carry out

	SOL1_BYTE EQ; //EQ
	SOL1_BYTE P;
	SOL1_BYTE G;
};

SOL1_BYTE sol1_alu_4bit_op(struct sol1_alu_4bit* alu, SOL1_BYTE A, SOL1_BYTE B, SOL1_BYTE CIN, SOL1_BYTE S, SOL1_BYTE M);

#endif