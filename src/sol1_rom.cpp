//
// sol1_rom.cpp
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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "sol1_rom.h"
#include "utils.h"

#if defined(__linux__) || defined(__MINGW32__)

#else

#endif

const char* SOL1_ROM_CONTROL_LIST[] = {
	"next_0", "next_1", "offset_0", "offset_1", "offset_2", "offset_3", "offset_4", "offset_5", "offset_6", "cond_inv", "cond_flags_src", "cond_sel_0",
	"cond_sel_1", "cond_sel_2", "cond_sel_3", "ESCAPE", "uzf_in_src_0", "uzf_in_src_1", "ucf_in_src_0", "ucf_in_src_1", "usf_in_src", "uof_in_src", "IR_wrt", "status_wrt",
	"shift_src_0", "shift_src_1", "shift_src_2", "zbus_out_src_0", "zbus_out_src_1", "alu_a_src_0", "alu_a_src_1", "alu_a_src_2", "alu_a_src_3", "alu_a_src_4", "alu_a_src_5",
	"alu_op_0", "alu_op_1", "alu_op_2", "alu_op_3", "alu_mode", "alu_cf_in_src_0", "alu_cf_in_src_1", "alu_cf_in_inv", "zf_in_src_0", "zf_in_src_1", "alu_cf_out_inv",
	"cf_in_src_0", "cf_in_src_1", "cf_in_src_2", "sf_in_src_0", "sf_in_src_1", "of_in_src_0", "of_in_src_1", "of_in_src_2", "rd", "wr", "alu_b_src_0", "alu_b_src_1",
	"alu_b_src_2", "display_reg_load", "dl_wrt", "dh_wrt", "cl_wrt", "ch_wrt", "bl_wrt", "bh_wrt", "al_wrt", "ah_wrt", "mdr_in_src", "mdr_out_src", "mdr_out_en",
	"mdrl_wrt", "mdrh_wrt", "tdrl_wrt", "tdrh_wrt", "dil_wrt", "dih_wrt", "sil_wrt", "sih_wrt", "marl_wrt", "marh_wrt", "bpl_wrt", "bph_wrt", "pcl_wrt", "pch_wrt",
	"spl_wrt", "sph_wrt", "-", "-", "int_vector_wrt", "mask_flags_wrt", "mar_in_src", "int_ack", "clear_all_ints", "ptb_wrt", "pagtbl_ram_we", "mdr_to_pagtbl_en",
	"force_user_ptb", "-", "-", "-", "-", "gl_wrt", "gh_wrt", "imm_0", "imm_1", "imm_2", "imm_3", "imm_4", "imm_5", "imm_6", "imm_7", "-", "-", "-", "-", "-", "-", "-", "-"
};



int SOL1_ROM::load_rom(const char* filename, SOL1_BYTE* rom, HW_TTY& hw_tty) {

	char str_out[255];
	sprintf(str_out, "The filename to load is: %s", filename);
	hw_tty.print(str_out);

	FILE* f = fopen(filename, "rb");
	if (!f)
	{
		hw_tty.print(" | Failed to open the file.\n");
		return 0;
	}

	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	fseek(f, 0, SEEK_SET);

	char* buf = (char*)malloc(size * sizeof(char));

	size_t res = fread(buf, size, 1, f);
	if (res != 1)
	{
		hw_tty.print(" | Failed to read from file.\n");
		return 0;
	}

	int i;
	for (i = 0; i < size; i++)
		rom[i] = buf[i];

	hw_tty.print(" | OK.\n");
	return 1;
}



void SOL1_ROM::is_rom_in_bounds(SOL1_MWORD index)
{
	assert(index >= 0 && index < SOL1_ROM_SIZE);
}

void SOL1_ROM::init(HW_TTY& hw_tty)
{
	int i = 0;

	this->rom_desc = (SOL1_BYTE*)malloc(sizeof(SOL1_BYTE) * SOL1_ROM_DESC);

	this->roms = (SOL1_BYTE**)malloc(sizeof(SOL1_BYTE*) * SOL1_ROM_DESC);

	load_rom("rom", this->rom_desc, hw_tty);

	int _roms = 0;
	for (_roms = 0; _roms < SOL1_ROM_NBR_ROMS; _roms++) {
		char filename[20];
		sprintf(filename, "rom%d", _roms);

		this->roms[_roms] = (SOL1_BYTE*)malloc(sizeof(SOL1_BYTE*) * (SOL1_ROM_NBR_ROMS *  SOL1_ROM_NBR_INSTRUCTIONS));
		load_rom(filename, this->roms[_roms], hw_tty);
	}

	this->MAR = 0x0;
	this->debug_mem_offset = 0;

	this->bkpt_opcode = 0x00;
	this->bkpt_cycle = 0x00;
}



void SOL1_ROM::display_current_cycles(SOL1_BYTE opcode, SOL1_BYTE cycle, SOL1_BYTE debug_desc_type, HW_TTY& hw_tty) {

	char str_out[255];
	int k, j, i;
	int p = opcode * SOL1_ROM_CYCLES_PER_INSTR + cycle;

	for (j = 0; j < SOL1_ROM_NBR_ROMS; j++) {

		if (j % 8 == 0) {
			hw_tty.print(" ");
			for (i = j; i < SOL1_ROM_NBR_ROMS && i < (j + 8); i++) {
				sprintf(str_out, " Rom %02d  ", i);
				hw_tty.print(str_out);
			}
			hw_tty.print("\n");
			hw_tty.print(" ");
		}

		hw_tty.print(" ");

		sprintf(str_out, "%c%c%c%c%c%c%c%c", INV_BYTE_TO_BINARY(this->roms[j][p]));
		hw_tty.print(str_out);

		if ((j + 1) % 8 == 0 && j < SOL1_ROM_NBR_ROMS - 1)
			hw_tty.print("\n\n");
	}

	hw_tty.print("\n\n");

	if (debug_desc_type == 1) {
		hw_tty.print("---------\n");
		if (strlen((const char*)(&this->rom_desc[(256 * 64 * opcode) + (256 * cycle)])) > 0) {
			sprintf(str_out, "%s\n", &this->rom_desc[(256 * 64 * opcode) + (256 * cycle)]);
			hw_tty.print(str_out);
			hw_tty.print("---------\n");
		}
	}
	else {
		for (j = 0; j < 24; j++)
		{
			hw_tty.print(" ");
			for (k = 0; k < 4; k++) {

				int c_rom = (j + 24 * k) / 8;
				int c_p = opcode * SOL1_ROM_CYCLES_PER_INSTR + cycle;
				char c_bit = pow(2, (j + 24 * k) % 8);
				unsigned char c_byte = this->roms[c_rom][c_p];

				char* tmp = (char*)malloc(strlen(SOL1_ROM_CONTROL_LIST[j + 24 * k] + 1));

				if ((c_byte & c_bit) >> ((j + 24 * k) % 8) == 1)
					sprintf(tmp, "*%s*", SOL1_ROM_CONTROL_LIST[j + 24 * k]);
				else
					sprintf(tmp, " %s ", SOL1_ROM_CONTROL_LIST[j + 24 * k]);

				sprintf(str_out, "%s", rightpad(tmp, 18));
				hw_tty.print(str_out);

			}
			hw_tty.print("\n");
		}
	}
	hw_tty.print("\n");

	sprintf(str_out, " Inst.: %02x | ", opcode); hw_tty.print(str_out);
	sprintf(str_out, "Cycle: %02x | ", cycle); hw_tty.print(str_out);
	sprintf(str_out, "%s\n", &this->rom_desc[0x400000 + (opcode * 256)]); hw_tty.print(str_out);

	hw_tty.print("\n");
}

void SOL1_ROM::display_current_cycles_desc(SOL1_BYTE opcode, SOL1_BYTE cycle, HW_TTY& hw_tty) {

	int p = opcode * SOL1_ROM_CYCLES_PER_INSTR + cycle;
	char str_out[255];

	sprintf(str_out, " *Inst.: %02x | ", opcode); hw_tty.print(str_out);
	sprintf(str_out, "Cycle: %02x | ", cycle); hw_tty.print(str_out);
	sprintf(str_out, "%s\n", &this->rom_desc[0x400000 + (opcode * 256)]); hw_tty.print(str_out);

	hw_tty.print("---------\n");
	if (strlen((const char*)(&this->rom_desc[(256 * 64 * opcode) + (256 * cycle)])) > 0) {
		sprintf(str_out, "%s\n", &this->rom_desc[(256 * 64 * opcode) + (256 * cycle)]);
		hw_tty.print(str_out);
		hw_tty.print("---------\n");
	}

}



void SOL1_ROM::menu(SOL1_BYTE debug_desc_type, HW_TTY& hw_tty) {
	hw_tty.print("\n");
	hw_tty.print("SOL-1 Debug Monitor > Roms > Microcode Cycles\n");
	hw_tty.print("\n");

	hw_tty.print("  S - Set Opcode\n");
	hw_tty.print("  D - Display current Cycle\n");
	hw_tty.print("  N - Next Cycle\n");
	hw_tty.print("  P - Previous Cycle\n");

	hw_tty.print("\n");

	if (debug_desc_type == 0)
		hw_tty.print("  T - Show Microcode Description \n");
	else
		hw_tty.print("  T - Show Microcode Settings \n");

	hw_tty.print("\n");

	hw_tty.print("  ? - Display Menu\n");
	hw_tty.print("  Q - Back to Rom Microcode Cycles\n");
	hw_tty.print("\n");
}


void SOL1_ROM::debug_cycles(HW_TTY& hw_tty) {

	hw_tty.print("Display Rom Microcode Cycles\n");

	SOL1_BYTE opcode = 0;
	SOL1_BYTE cycle = 0;

	SOL1_BYTE debug_desc_type = 0;

	while (1) {


		hw_tty.print("roms/microcode cycles> ");
		int key = hw_tty.get_char();


		if (key == 'n' || key == 'N') {
			if (cycle < SOL1_ROM_CYCLES_PER_INSTR - 1)
				cycle++;
			hw_tty.print("\n\n");
			display_current_cycles(opcode, cycle, debug_desc_type, hw_tty);
		}
		else if (key == 'P' || key == 'P') {
			if (cycle > 0)
				cycle--;
			hw_tty.print("\n\n");
			display_current_cycles(opcode, cycle, debug_desc_type, hw_tty);
		}
		else if (key == 't' || key == 'T') {
			debug_desc_type = (debug_desc_type == 0);

			if (debug_desc_type == 0)
				hw_tty.print("Showing Microcode Settings\n");
			else
				hw_tty.print("Showing Microcode Description \n");

		}
		else if (key == 'q' || key == 'Q') {
			hw_tty.print("\n");
			return;
		}
		else if (key == '?') {
			SOL1_ROM::menu(debug_desc_type, hw_tty);
		}
		else if (key == 's' || key == 'S') {

			char *input;
			char *value = (char*)malloc(sizeof(char) * 257);

			hw_tty.print("Opcode ? ");
			input = hw_tty.gets(2);
			leftpad(input, value, 2);

			opcode = convert_hexstr_to_value(value);
			cycle = 0;
			hw_tty.print("\n\n");
			display_current_cycles(opcode, cycle, debug_desc_type, hw_tty);

			free(input);
			free(value);
		}
		else if (key == 'd' || key == 'D') {
			hw_tty.print("\n\n");
			display_current_cycles(opcode, cycle, debug_desc_type, hw_tty);
		}
		else
			hw_tty.print("\n");

	}
}

