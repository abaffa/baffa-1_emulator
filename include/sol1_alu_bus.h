//
// sol1_alu_bus.h
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
#ifndef SOL1ALUBUS_H
#define SOL1ALUBUS_H

#include "config.h"

struct sol1_alu_bus {
	SOL1_BYTE x_bus; //alu entrada
	SOL1_BYTE y_bus; //alu entrada

	SOL1_BYTE z_bus; //alu saida

////////////////////////////////////////////////
	SOL1_BYTE alu_zf; // flags do alu
	SOL1_BYTE alu_cf;
	SOL1_BYTE alu_of;

	SOL1_BYTE alu_final_cf;
	SOL1_BYTE alu_output;

};

#endif