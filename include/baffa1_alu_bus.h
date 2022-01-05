//
// baffa1_alu_bus.h
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
#ifndef BAFFA1ALUBUS_H
#define BAFFA1ALUBUS_H

#include "config.h"

struct baffa1_alu_bus {
	BAFFA1_BYTE x_bus; //alu entrada
	BAFFA1_BYTE y_bus; //alu entrada

	BAFFA1_BYTE z_bus; //alu saida

////////////////////////////////////////////////
	BAFFA1_BYTE alu_zf; // flags do alu
	BAFFA1_BYTE alu_cf;
	BAFFA1_BYTE alu_of;

	BAFFA1_BYTE alu_final_cf;
	BAFFA1_BYTE alu_output;

};

#endif