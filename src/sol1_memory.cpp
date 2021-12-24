//
// sol1_memory.cpp
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
#include "sol1_memory.h"
#include <stdlib.h>
#include <assert.h>
#include "utils.h"

SOL1_MEMORY::SOL1_MEMORY()
{

	int i = 0;
	this->mem_bios = (SOL1_BYTE*)malloc(SOL1_BIOS_MEMORY_SIZE * sizeof(SOL1_BYTE));

	this->mem_page_table0 = (SOL1_BYTE*)malloc(SOL1_PAGING_MEMORY_SIZE * sizeof(SOL1_BYTE));
	this->mem_page_table1 = (SOL1_BYTE*)malloc(SOL1_PAGING_MEMORY_SIZE * sizeof(SOL1_BYTE));

	this->low_memory = (SOL1_BYTE*)malloc(SOL1_MAIN_MEMORY_SIZE * sizeof(SOL1_BYTE));

	this->high_memory0 = (SOL1_BYTE*)malloc(SOL1_MAIN_MEMORY_SIZE * sizeof(SOL1_BYTE));
	this->high_memory1 = (SOL1_BYTE*)malloc(SOL1_MAIN_MEMORY_SIZE * sizeof(SOL1_BYTE));
	this->high_memory2 = (SOL1_BYTE*)malloc(SOL1_MAIN_MEMORY_SIZE * sizeof(SOL1_BYTE));
	this->high_memory3 = (SOL1_BYTE*)malloc(SOL1_MAIN_MEMORY_SIZE * sizeof(SOL1_BYTE));
	this->high_memory4 = (SOL1_BYTE*)malloc(SOL1_MAIN_MEMORY_SIZE * sizeof(SOL1_BYTE));
	this->high_memory5 = (SOL1_BYTE*)malloc(SOL1_MAIN_MEMORY_SIZE * sizeof(SOL1_BYTE));
	this->high_memory6 = (SOL1_BYTE*)malloc(SOL1_MAIN_MEMORY_SIZE * sizeof(SOL1_BYTE));
	this->high_memory7 = (SOL1_BYTE*)malloc(SOL1_MAIN_MEMORY_SIZE * sizeof(SOL1_BYTE));

	this->debug_mem_offset = 0;
	this->debug_manual_offset = 0x00;

	this->reset();
}

void SOL1_MEMORY::reset()
{
	int address = 0;

	for (address = 0; address < SOL1_BIOS_MEMORY_SIZE; address++) {
		this->mem_bios[address] = 0x00;
	}
	for (address = 0; address < SOL1_PAGING_MEMORY_SIZE; address++) {
		this->mem_page_table0[address] = 0x00;
		this->mem_page_table1[address] = 0x00;
	}
	for (address = 0; address < SOL1_MAIN_MEMORY_SIZE; address++) {
		this->low_memory[address] = 0x00;

		this->high_memory0[address] = 0x00;
		this->high_memory1[address] = 0x00;
		this->high_memory2[address] = 0x00;
		this->high_memory3[address] = 0x00;
		this->high_memory4[address] = 0x00;
		this->high_memory5[address] = 0x00;
		this->high_memory6[address] = 0x00;
		this->high_memory7[address] = 0x00;

	}

}


int SOL1_MEMORY::load_bios() {
	char str_out[255];
	int i;
	long size = 0;
	char * buf = loadfile(str_out, (char*)"bios.obj", &size);

	if (buf == NULL)
		return 0;

	for (i = 0; i < size; i++) {
		this->mem_bios[i] = buf[i];
	}

	return 1;
}


void SOL1_MEMORY::displayBiosMemory(SOL1_REGISTERS& registers, HW_TTY& hw_tty)
{
	char str_out[255];
	int i = 0;
	SOL1_DWORD MAR = SOL1_REGISTERS::value(registers.MARl, registers.MARh);

	if (this->debug_manual_offset == 0x00) {
		if (0 + this->debug_mem_offset > MAR || MAR >= 256 + this->debug_mem_offset)
			this->debug_mem_offset = (MAR / 0x10) * 0x10;
	}

	SOL1_BYTE mem_bytes[256];
	for (i = 0; i < 256; i++) {
		mem_bytes[i] = this->mem_bios[i + this->debug_mem_offset];
	}

	this->displayMemory(mem_bytes, this->debug_mem_offset, MAR, hw_tty);
}

void SOL1_MEMORY::displayLowMemory(SOL1_REGISTERS& registers, HW_TTY& hw_tty)
{
	char str_out[255];
	int i = 0;
	SOL1_DWORD MAR = SOL1_REGISTERS::value(registers.MARl, registers.MARh);

	if (this->debug_manual_offset == 0x00) {
		if (0 + this->debug_mem_offset > MAR || MAR >= 256 + this->debug_mem_offset)
			this->debug_mem_offset = (MAR / 0x10) * 0x10;
	}

	SOL1_BYTE mem_bytes[256];
	for (i = 0; i < 256; i++) {
		mem_bytes[i] = this->low_memory[i + this->debug_mem_offset];
	}

	this->displayMemory(mem_bytes, this->debug_mem_offset, MAR, hw_tty);
}


void SOL1_MEMORY::displayMainMemory(SOL1_REGISTERS& registers, HW_TTY& hw_tty)
{
	char str_out[255];
	int i = 0;
	SOL1_DWORD MAR = SOL1_REGISTERS::value(registers.MARl, registers.MARh);

	if (this->debug_manual_offset == 0x00) {
		if (0 + this->debug_mem_offset > MAR || MAR >= 256 + this->debug_mem_offset)
			this->debug_mem_offset = (MAR / 0x10) * 0x10;
	}

	SOL1_BYTE mem_bytes[256];
	for (i = 0; i < 256; i++) {
		if (i + this->debug_mem_offset > 0x7FFF)
			mem_bytes[i] = this->low_memory[i + this->debug_mem_offset];
		else
			mem_bytes[i] = this->mem_bios[i + this->debug_mem_offset];
	}

	this->displayMemory(mem_bytes, this->debug_mem_offset, MAR, hw_tty);
}



void SOL1_MEMORY::displayMemory(SOL1_BYTE* mem_bytes, int offset, SOL1_MWORD MAR, HW_TTY& hw_tty)
{
	char str_out[255];
	int i = 0, j = 0;

	hw_tty.print("\n        ");

	for (i = 0; i < 16; i++) {
		sprintf(str_out, "%02x ", i);
		hw_tty.print(str_out);
	}

	sprintf(str_out, "\n\n %04x ", offset); hw_tty.print(str_out);

	for (i = 0; i < 256; i++) {
		if (i % 16 == 0)
			if (MAR == i)
				hw_tty.print(" *");
			else
				hw_tty.print("  ");
		if (MAR == i + offset || MAR - 1 == i + offset) {
			sprintf(str_out, "%02x*", mem_bytes[i]); hw_tty.print(str_out);
		}
		else {
			sprintf(str_out, "%02x ", mem_bytes[i]); hw_tty.print(str_out);
		}

		if ((i + 1) % 16 == 0 && i <= 255) {
			hw_tty.print("  |");
			for (j = (i + 1) - 16; j < (i + 1); j++) {
				if (mem_bytes[j] < 0x20)
					hw_tty.print(".");
				else {
					sprintf(str_out, "%c", mem_bytes[j]);
					hw_tty.print(str_out);
				}
			}
			hw_tty.print("|");

			if (i < 255) {
				sprintf(str_out, "\n %04x ", i + offset + 1);
				hw_tty.print(str_out);
			}
			else
				hw_tty.print("\n");

		}
	}
}



void SOL1_MEMORY::display_test(SOL1_REGISTERS&  registers, HW_TTY& hw_tty)
{
	char str_out[255];
	int i = 0, j = 0;
	SOL1_DWORD MAR = SOL1_REGISTERS::value(registers.MARl, registers.MARh);

	if (this->debug_manual_offset == 0x00) {
		if (0 + this->debug_mem_offset > MAR || MAR >= 256 + this->debug_mem_offset)
			this->debug_mem_offset = (MAR / 0x10) * 0x10;
	}

	hw_tty.print("\n        ");

	for (i = 0; i < 16; i++) {
		sprintf(str_out, "%02x ", i);
		hw_tty.print(str_out);
	}

	sprintf(str_out, "\n\n %04x ", this->debug_mem_offset); hw_tty.print(str_out);

	for (i = 0 + this->debug_mem_offset; i < 256 + this->debug_mem_offset; i++) {
		if (i % 16 == 0)
			if (MAR == i)
				hw_tty.print(" *");
			else
				hw_tty.print("  ");
		if (MAR == i || MAR - 1 == i) {
			sprintf(str_out, "%02x*", this->low_memory[i]);
			hw_tty.print(str_out);
		}
		else {
			sprintf(str_out, "%02x ", this->low_memory[i]);
			hw_tty.print(str_out);
		}

		if ((i + 1) % 16 == 0 && i <= 255 + this->debug_mem_offset) {
			hw_tty.print("  |");
			for (j = (i + 1) - 16; j < (i + 1); j++) {
				if (this->low_memory[j] < 0x20)
					hw_tty.print(".");
				else {
					sprintf(str_out, "%c", this->low_memory[j]);
					hw_tty.print(str_out);
				}
			}
			hw_tty.print("|");

			if (i < 255 + this->debug_mem_offset) {
				sprintf(str_out, "\n %04x ", i + 1);
				hw_tty.print(str_out);
			}
			else
				hw_tty.print("\n");

		}
	}


}
