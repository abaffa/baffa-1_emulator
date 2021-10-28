//
// sol1_microcode.cpp
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
#include<stdio.h>

#include "sol1_microcode.h"
#include "utils.h"


void SOL1_MICROCODE::init(HW_TTY& hw_tty) {

	controller_bus.u_escape_0 = 0x00;
	controller_bus.u_escape_1 = 0x00;

	controller_bus.u_esc_in_src = 0x00;

	controller_bus.u_offset = 0b00000000;
	controller_bus.uzf_in_src = 0x00;
	controller_bus.ucf_in_src = 0x00;
	controller_bus.usf_in_src = 0x00;
	controller_bus.uof_in_src = 0x00;

	this->u_adder_b = 0x00;
	this->u_ad_bus = 0x00;
	this->u_adder = 0x00;

	this->IR.reset();

	this->U_ADDRESSl.reset();
	this->U_ADDRESSh.reset();

	///////////////////////////////////////
	controller_bus.next = 0x00;

	controller_bus.cond_inv = 0x00;
	controller_bus.cond_flags_src = 0x00;
	controller_bus.cond_sel = 0x00;

	controller_bus.ir_wrt = 0x00;

	controller_bus.shift_src = 0x00;
	controller_bus.zbus_out_src = 0x00;

	controller_bus.zf_in_src = 0x00;
	controller_bus.cf_in_src = 0x00;
	controller_bus.sf_in_src = 0x00;
	controller_bus.of_in_src = 0x00;

	controller_bus.rd = 0x00;
	controller_bus.wr = 0x00;


	controller_bus.display_reg_load = 0x00;
	controller_bus.dl_wrt = 0x00;
	controller_bus.dh_wrt = 0x00;
	controller_bus.cl_wrt = 0x00;
	controller_bus.ch_wrt = 0x00;
	controller_bus.bl_wrt = 0x00;
	controller_bus.bh_wrt = 0x00;
	controller_bus.al_wrt = 0x00;
	controller_bus.ah_wrt = 0x00;
	controller_bus.mdr_in_src = 0x00;
	controller_bus.mdr_out_src = 0x00;
	controller_bus.mdr_out_en = 0x00;
	controller_bus.mdrl_wrt = 0x00;
	controller_bus.mdrh_wrt = 0x00;
	controller_bus.tdrl_wrt = 0x00;
	controller_bus.tdrh_wrt = 0x00;
	controller_bus.dil_wrt = 0x00;
	controller_bus.dih_wrt = 0x00;
	controller_bus.sil_wrt = 0x00;
	controller_bus.sih_wrt = 0x00;
	controller_bus.marl_wrt = 0x00;
	controller_bus.marh_wrt = 0x00;
	controller_bus.bpl_wrt = 0x00;
	controller_bus.bph_wrt = 0x00;
	controller_bus.pcl_wrt = 0x00;
	controller_bus.pch_wrt = 0x00;
	controller_bus.spl_wrt = 0x00;
	controller_bus.sph_wrt = 0x00;

	controller_bus.mar_in_src = 0x00;

	controller_bus.ptb_wrt = 0x00;
	controller_bus.pagtbl_ram_we = 0x00;
	controller_bus.mdr_to_pagtbl_en = 0x00;
	controller_bus.force_user_ptb = 0x00;

	controller_bus.gl_wrt = 0x00;
	controller_bus.gh_wrt = 0x00;
	controller_bus.imm = 0x00;

	//controller_bus.sspl_wrt = 0x01; ////////?????????????????
	//controller_bus.ssph_wrt = 0x01; ////////?????????????????

	//?????????????
	controller_bus.memory_io = 0x00; // bus_mem_io //?????????????????
	controller_bus.page_present = 0x00; ////////?????????????????
	controller_bus.page_writable = 0x00; ////////?????????????????

	controller_bus.status_wrt = 0x00; //mswl_wrt // status (flags de controle)

	//
	controller_bus.clear_all_ints = 0x00;
	controller_bus.int_vector = 0x00; ////////?????????????????
	controller_bus.mask_flags_wrt = 0x00;
	controller_bus.int_status = 0x00; ////////?????????????????
	controller_bus.int_vector_wrt = 0x00;
	controller_bus.int_ack = 0x00;
	controller_bus.int_request = 0x00; ////////?????????????????
	controller_bus.int_req = 0x00;
	//

	controller_bus.dma_req = 0x00; ////////????????????????? CLOCK DE DMA

	controller_bus.wait = 0x00; ////////?????????????????
	controller_bus.ext_input = 0x00; ////////?????????????????

	controller_bus.final_condition = 0x00;

	controller_bus.panel_regsel = 0x00;

	controller_bus.panel_rd = 0x01;
	controller_bus.panel_wr = 0x00;
	controller_bus.panel_mem_io = 0x00;

	controller_bus.panel_address = 0x00;
	controller_bus.panel_data = 0x00;
	controller_bus.panel_req = 0x00;

	controller_bus.panel_run = 0x00;
	controller_bus.panel_step = 0x000;
	controller_bus.panel_microcodestep = 0x00;

	controller_bus.clk = 0x00;


	controller_bus.reset = 0x00;
	controller_bus.restart = 0x00;


	this->rom.init(hw_tty);
}





SOL1_BYTE SOL1_MICROCODE::int_pending(SOL1_BYTE reg_status_value) {
	return get_byte_bit(controller_bus.int_request, 0) & get_byte_bit(reg_status_value, MSWl_INTERRUPT_ENABLE);
}

SOL1_BYTE SOL1_MICROCODE::any_interruption(SOL1_BYTE reg_status_value) {
	return get_byte_bit(int_pending(reg_status_value), 0) | get_byte_bit(controller_bus.dma_req, 0);
}

SOL1_BYTE SOL1_MICROCODE::page_table_addr_src(SOL1_BYTE reg_status_value) {
	return get_byte_bit(controller_bus.force_user_ptb, 0) ^ get_byte_bit(reg_status_value, MSWl_CPU_MODE);
}


SOL1_BYTE SOL1_MICROCODE::MUX(SOL1_BYTE reg_status_value) {

	SOL1_BYTE mux_A = (controller_bus.next == 0b00000011) || (controller_bus.next == 0b00000010 && any_interruption(reg_status_value) == 0x01);
	SOL1_BYTE mux_B = controller_bus.next == 0b00000010;

	SOL1_BYTE mux = set_byte_bit(mux_B, 1) | set_byte_bit(mux_A, 0);

	return mux;
}


void SOL1_MICROCODE::display_u_adder(SOL1_BYTE typ, SOL1_BYTE final_condition, HW_TTY& hw_tty) {

	char str_out[255];
	hw_tty.print("* next(typ): "); print_nibble_bin(str_out, typ); hw_tty.print(str_out);
	hw_tty.print(" | ");
	hw_tty.print(" u_offset: "); print_byte_bin(str_out, controller_bus.u_offset); hw_tty.print(str_out);
	hw_tty.print(" | ");
	hw_tty.print("Final Condition : "); print_nibble_bin(str_out, final_condition); hw_tty.print(str_out);
	hw_tty.print("\n");

	hw_tty.print("* A(u_ad): ");  print_word_bin_nibbles(str_out, this->u_ad_bus); hw_tty.print(str_out);
	hw_tty.print("\n");
	hw_tty.print("* B: ");  print_word_bin_nibbles(str_out, this->u_adder_b); hw_tty.print(str_out);
	hw_tty.print("\n");
	hw_tty.print("* u_adder: ");  print_word_bin(str_out, this->u_adder); hw_tty.print(str_out);
	hw_tty.print("\n");
}

SOL1_DWORD SOL1_MICROCODE::u_adder_refresh(SOL1_BYTE typ, SOL1_BYTE final_condition, SOL1_CONFIG& config, HW_TTY& hw_tty) {

	//if type = branch, and condition = false, then next = +1

	//Typ1 Typ0		Desc
	//0    0		offset
	//0    1		branch
	//1    0		pre-fetch
	//1    1		post_fetch

	SOL1_BYTE u_offset6 = get_byte_bit(controller_bus.u_offset, 6);

	if (typ == 0b00000001 && !check_byte_bit(final_condition, 0))
		this->u_adder_b = 0x01;
	else
		this->u_adder_b = controller_bus.u_offset |
		set_word_bit(u_offset6, 7) |
		set_word_bit(u_offset6, 8) |
		set_word_bit(u_offset6, 9) |
		set_word_bit(u_offset6, 10) |
		set_word_bit(u_offset6, 11) |
		set_word_bit(u_offset6, 12) |
		set_word_bit(u_offset6, 13);

	this->u_adder = (this->u_ad_bus & 0b0011111111111111) + this->u_adder_b;

	if (config.DEBUG_UADDER) {
		hw_tty.print("***** U_ADDER\n");
		display_u_adder(controller_bus.next, controller_bus.final_condition, hw_tty);
		hw_tty.print("\n");
	}

	return this->u_adder;
}


// Sets u-address
void SOL1_MICROCODE::addresser(SOL1_BYTE reg_status_value, SOL1_CONFIG& config, HW_TTY& hw_tty) {

	////////////////////////////////////////////////////////////////////////////
	//IC15 //IC59 //IC153 //IC167 //IC61 //IC12 //IC341 //IC175

	//update_final_condition(sol1_cpu);


	//if type = branch, and condition = false, then next = +1
	/*
	em 2 campos chamados next_0 e next_1.
		Paulo enviou 7 de maio às 02:37
		next = 00 : proximo ciclo definido pelo valor OFFSET
		next = 01 : usado para branches
		next = 10 : proximo ciclo eh instruction fetch
		next = 11 : proximo ciclo definido por IR
		*/
		//Typ1 Typ0		Desc
		//0    0		offset
		//0    1		branch
		//1    0		pre-fetch
		//1    1		post_fetch

	//IC45A //IC94B //IC119A //IC43D //IC45A //IC43D //IC34 //IC35 //IC36 //IC37 //IC38 //IC40 //IC73


		//MICROCODE 0 TO 63
	//IS RESERVED FOR CPU STARTUP.
	//U_FETCH AT 0x10
	//U_TRAP AT 0x20
	SOL1_DWORD u_fetch = 0x10;
	SOL1_DWORD u_trap = 0x20;

	SOL1_BYTE mux = MUX(reg_status_value);


	SOL1_DWORD u_address_mux_l = 0x00;
	SOL1_DWORD u_address_mux_h = 0x00;

	if (mux == 0x00) {
		u_address_mux_l = this->u_adder & 0b00000011111111;
		u_address_mux_h = (this->u_adder & 0b11111100000000) >> 8;
	}
	else if (mux == 0x01) {
		u_address_mux_l = (get_byte_bit(controller_bus.u_escape_0, 0) << 4) | (get_byte_bit(controller_bus.u_escape_1, 0) << 5) | ((this->IR.value() & 0b00000011) << 6);
		u_address_mux_h = ((this->IR.value() & 0b11111100) >> 2);
	}
	else if (mux == 0x02) {
		u_address_mux_l = u_fetch & 0b00000011111111;
		u_address_mux_h = (u_fetch & 0b11111100000000) >> 8;
	}
	else if (mux == 0x03) {
		u_address_mux_l = u_trap & 0b00000011111111;
		u_address_mux_h = (u_trap & 0b11111100000000) >> 8;
	}


	SOL1_DWORD u_address = (((SOL1_DWORD)u_address_mux_h) << 8) | u_address_mux_l;
	SOL1_REGISTERS::set(this->U_ADDRESSl, this->U_ADDRESSh, u_address);

	this->old_u_ad_bus = this->u_ad_bus;

	if (controller_bus.reset == 0x00) 
		this->u_ad_bus = u_address;
	else
		this->u_ad_bus = 0;




	if (config.DEBUG_UADDRESSER) {
		char str_out[255];
		hw_tty.print("***** U_ADDRESSER\n");
		hw_tty.print("* Next(typ): "); print_nibble_bin(str_out, controller_bus.next); hw_tty.print(str_out);
		hw_tty.print(" | Any Interruption: "); print_nibble_bin(str_out, any_interruption(reg_status_value)); hw_tty.print(str_out);
		//hw_tty.print(" | Mux_B: "); print_nibble_bin(str_out, mux_B); hw_tty.print(str_out);
		//hw_tty.print(" | Mux_A: "); print_nibble_bin(str_out, mux_A); hw_tty.print(str_out);
		hw_tty.print("\n");

		/*
		hw_tty.print("* "); print_byte_bin(str_out, in01_A); hw_tty.print(str_out); hw_tty.print(" | "); print_byte_bin(str_out, in01_B); hw_tty.print(str_out); hw_tty.print(" | "); print_byte_bin(str_out, res01); hw_tty.print(str_out); hw_tty.print("\n");
		hw_tty.print("* "); print_byte_bin(str_out, in02_A); hw_tty.print(str_out); hw_tty.print(" | "); print_byte_bin(str_out, in02_B); hw_tty.print(str_out); hw_tty.print(" | "); print_byte_bin(str_out, res02); hw_tty.print(str_out); hw_tty.print("\n");
		hw_tty.print("* "); print_byte_bin(str_out, in03_A); hw_tty.print(str_out); hw_tty.print(" | "); print_byte_bin(str_out, in03_B); hw_tty.print(str_out); hw_tty.print(" | "); print_byte_bin(str_out, res03); hw_tty.print(str_out); hw_tty.print("\n");
		hw_tty.print("* "); print_byte_bin(str_out, in04_A); hw_tty.print(str_out); hw_tty.print(" | "); print_byte_bin(str_out, in04_B); hw_tty.print(str_out); hw_tty.print(" | "); print_byte_bin(str_out, res04); hw_tty.print(str_out); hw_tty.print("\n");
		hw_tty.print("* "); print_byte_bin(str_out, in05_A); hw_tty.print(str_out); hw_tty.print(" | "); print_byte_bin(str_out, in05_B); hw_tty.print(str_out); hw_tty.print(" | "); print_byte_bin(str_out, res05); hw_tty.print(str_out); hw_tty.print("\n");
		hw_tty.print("* "); print_byte_bin(str_out, in06_A); hw_tty.print(str_out); hw_tty.print(" | "); print_byte_bin(str_out, in06_B); hw_tty.print(str_out); hw_tty.print(" | "); print_byte_binstr_out, res06); hw_tty.print(str_out); hw_tty.print("\n");
		hw_tty.print("* "); print_byte_bin(str_out, in07_A); hw_tty.print(str_out); hw_tty.print(" | "); print_byte_bin(str_out, in07_B); hw_tty.print(str_out); hw_tty.print(" | "); print_byte_bin(str_out, res07); hw_tty.print(str_out); hw_tty.print("\n");
		*/
		sprintf(str_out, "* U_ADDRESS=%04x", SOL1_REGISTERS::value(this->U_ADDRESSl, this->U_ADDRESSh)); hw_tty.print(str_out);
		hw_tty.print(" | ");
		sprintf(str_out, "U_AD=%04x", this->u_ad_bus); hw_tty.print(str_out);
		hw_tty.print(" | ");
		hw_tty.print("Mux: "); print_nibble_bin(str_out, mux); hw_tty.print(str_out);
		hw_tty.print("\n");

		hw_tty.print("\n");
	}

	//return this->u_ad_bus;
}

// Sets Microcode
void SOL1_MICROCODE::sequencer_update(SOL1_BYTE reg_status_value, SOL1_CONFIG& config, HW_TTY& hw_tty) {

	//CLOCK LOW
	this->addresser(reg_status_value, config, hw_tty); // Sets u-address

	////////////////////////////////////////////////////////////////////////////

	this->load_microcode_from_rom(); // Sets Microcode from Rom

	if (config.DEBUG_MICROCODE) {
		char str_out[255];
		hw_tty.print("***** MICROCODE\n");
		//hw_tty.print("U-ADDRESS: ");  print_word_bin(str_out, this->u_ad_bus); hw_tty.print(str_out); hw_tty.print("\n");		
		//sprintf(str_out, "OPCODE: %02x (cycle %02x)\n", (this->u_ad_bus / 64), (this->u_ad_bus % 64)); hw_tty.print(str_out);
		//hw_tty.print("microcode: \n");
		this->rom.display_current_cycles_desc((this->u_ad_bus / 64), (this->u_ad_bus % 64), hw_tty);
		hw_tty.print("\n");
	}
}

// Sets Microcode from Rom
void SOL1_MICROCODE::load_microcode_from_rom() {

	if (controller_bus.reset == 0x00) {

		// ROM 0
		controller_bus.next = this->rom.roms[0][this->u_ad_bus] & 0b00000011;///////////////////////

		// ROM 1
		controller_bus.u_offset = ((this->rom.roms[1][this->u_ad_bus] & 0b00000001) << 6) | ((this->rom.roms[0][this->u_ad_bus] >> 2) & 0b00111111);////////////////////
		controller_bus.cond_inv = (this->rom.roms[1][this->u_ad_bus] >> 1) & 0b00000001;
		controller_bus.cond_flags_src = (this->rom.roms[1][this->u_ad_bus] >> 2) & 0b00000001;
		controller_bus.cond_sel = (this->rom.roms[1][this->u_ad_bus] >> 3) & 0b00001111;
		controller_bus.u_escape_0 = this->rom.roms[1][this->u_ad_bus] >> 7 & 0b00000001; //ESCAPE

		// ROM 2
		controller_bus.uzf_in_src = this->rom.roms[2][this->u_ad_bus] & 0b00000011;/////////////////
		controller_bus.ucf_in_src = (this->rom.roms[2][this->u_ad_bus] >> 2) & 0b00000011;//////////
		controller_bus.usf_in_src = (this->rom.roms[2][this->u_ad_bus] >> 4) & 0b00000001;//////////
		controller_bus.uof_in_src = (this->rom.roms[2][this->u_ad_bus] >> 5) & 0b00000001;//////////
		controller_bus.ir_wrt = (this->rom.roms[2][this->u_ad_bus] >> 6) & 0b00000001;/////////////////
		controller_bus.status_wrt = (this->rom.roms[2][this->u_ad_bus] >> 7) & 0b00000001;/////////////

		// ROM 3
		controller_bus.shift_src = this->rom.roms[3][this->u_ad_bus] & 0b00000111;/////////////////////
		controller_bus.zbus_out_src = (this->rom.roms[3][this->u_ad_bus] >> 3) & 0b00000011;///////////
		controller_bus.alu_a_src = ((this->rom.roms[3][this->u_ad_bus] >> 5) & 0b00000111) | ((this->rom.roms[4][this->u_ad_bus] & 0b00000111) << 3);/////////////////////

		// ROM 4
		controller_bus.alu_op = (this->rom.roms[4][this->u_ad_bus] >> 3) & 0b00001111;//////////////
		controller_bus.alu_mode = (this->rom.roms[4][this->u_ad_bus] >> 7) & 0b00000001;////////////

		// ROM 5
		controller_bus.alu_cf_in_src = (this->rom.roms[5][this->u_ad_bus]) & 0b00000011;////////////
		controller_bus.alu_cf_in_inv = (this->rom.roms[5][this->u_ad_bus] >> 2) & 0b00000001;///////
		controller_bus.zf_in_src = (this->rom.roms[5][this->u_ad_bus] >> 3) & 0b00000011;///////////////
		controller_bus.alu_cf_out_inv = (this->rom.roms[5][this->u_ad_bus] >> 5) & 0b00000001;//////
		controller_bus.cf_in_src = ((this->rom.roms[5][this->u_ad_bus] >> 6) & 0b00000011) | ((this->rom.roms[6][this->u_ad_bus] & 0b00000001) << 2);/////////////////////

		// ROM 6
		controller_bus.sf_in_src = (this->rom.roms[6][this->u_ad_bus] >> 1) & 0b00000011;///////////////
		controller_bus.of_in_src = (this->rom.roms[6][this->u_ad_bus] >> 3) & 0b00000111;///////////////
		controller_bus.rd = (this->rom.roms[6][this->u_ad_bus] >> 6) & 0b00000001;//////////////////////
		controller_bus.wr = (this->rom.roms[6][this->u_ad_bus] >> 7) & 0b00000001;//////////////////////

		// ROM 7
		controller_bus.alu_b_src = this->rom.roms[7][this->u_ad_bus] & 0b00000111;//////////////////
		controller_bus.display_reg_load = (this->rom.roms[7][this->u_ad_bus] >> 3) & 0b00000001;////
		controller_bus.dl_wrt = (this->rom.roms[7][this->u_ad_bus] >> 4) & 0b00000001;//////////////
		controller_bus.dh_wrt = (this->rom.roms[7][this->u_ad_bus] >> 5) & 0b00000001;//////////////
		controller_bus.cl_wrt = (this->rom.roms[7][this->u_ad_bus] >> 6) & 0b00000001;//////////////
		controller_bus.ch_wrt = (this->rom.roms[7][this->u_ad_bus] >> 7) & 0b00000001;//////////////

		// ROM 8
		controller_bus.bl_wrt = (this->rom.roms[8][this->u_ad_bus] >> 0) & 0b00000001;//////////////
		controller_bus.bh_wrt = (this->rom.roms[8][this->u_ad_bus] >> 1) & 0b00000001;//////////////
		controller_bus.al_wrt = (this->rom.roms[8][this->u_ad_bus] >> 2) & 0b00000001;//////////////
		controller_bus.ah_wrt = (this->rom.roms[8][this->u_ad_bus] >> 3) & 0b00000001;//////////////
		controller_bus.mdr_in_src = (this->rom.roms[8][this->u_ad_bus] >> 4) & 0b00000001;//////////
		controller_bus.mdr_out_src = (this->rom.roms[8][this->u_ad_bus] >> 5) & 0b00000001;/////////
		controller_bus.mdr_out_en = (this->rom.roms[8][this->u_ad_bus] >> 6) & 0b00000001;//////////
		controller_bus.mdrl_wrt = (this->rom.roms[8][this->u_ad_bus] >> 7) & 0b00000001;////////////

		// ROM 9
		controller_bus.mdrh_wrt = (this->rom.roms[9][this->u_ad_bus] >> 0) & 0b00000001;////////////
		controller_bus.tdrl_wrt = (this->rom.roms[9][this->u_ad_bus] >> 1) & 0b00000001;////////////
		controller_bus.tdrh_wrt = (this->rom.roms[9][this->u_ad_bus] >> 2) & 0b00000001;////////////
		controller_bus.dil_wrt = (this->rom.roms[9][this->u_ad_bus] >> 3) & 0b00000001;/////////////
		controller_bus.dih_wrt = (this->rom.roms[9][this->u_ad_bus] >> 4) & 0b00000001;/////////////
		controller_bus.sil_wrt = (this->rom.roms[9][this->u_ad_bus] >> 5) & 0b00000001;/////////////
		controller_bus.sih_wrt = (this->rom.roms[9][this->u_ad_bus] >> 6) & 0b00000001;/////////////
		controller_bus.marl_wrt = (this->rom.roms[9][this->u_ad_bus] >> 7) & 0b00000001;////////////

		// ROM 10
		controller_bus.marh_wrt = (this->rom.roms[10][this->u_ad_bus] >> 0) & 0b00000001;///////////
		controller_bus.bpl_wrt = (this->rom.roms[10][this->u_ad_bus] >> 1) & 0b00000001;////////////
		controller_bus.bph_wrt = (this->rom.roms[10][this->u_ad_bus] >> 2) & 0b00000001;////////////
		controller_bus.pcl_wrt = (this->rom.roms[10][this->u_ad_bus] >> 3) & 0b00000001;////////////
		controller_bus.pch_wrt = (this->rom.roms[10][this->u_ad_bus] >> 4) & 0b00000001;////////////
		controller_bus.spl_wrt = (this->rom.roms[10][this->u_ad_bus] >> 5) & 0b00000001;////////////
		controller_bus.sph_wrt = (this->rom.roms[10][this->u_ad_bus] >> 6) & 0b00000001;////////////
		controller_bus.u_escape_1 = (this->rom.roms[10][this->u_ad_bus] >> 7) & 0b00000001;////////////

		// ROM 11
		controller_bus.u_esc_in_src = (this->rom.roms[11][this->u_ad_bus] >> 0) & 0b00000001;/////
		controller_bus.int_vector_wrt = (this->rom.roms[11][this->u_ad_bus] >> 1) & 0b00000001;/////
		controller_bus.mask_flags_wrt = (this->rom.roms[11][this->u_ad_bus] >> 2) & 0b00000001;/////
		controller_bus.mar_in_src = (this->rom.roms[11][this->u_ad_bus] >> 3) & 0b00000001;/////////
		controller_bus.int_ack = (this->rom.roms[11][this->u_ad_bus] >> 4) & 0b00000001;
		controller_bus.clear_all_ints = (this->rom.roms[11][this->u_ad_bus] >> 5) & 0b00000001;
		controller_bus.ptb_wrt = (this->rom.roms[11][this->u_ad_bus] >> 6) & 0b00000001;////////////
		controller_bus.pagtbl_ram_we = (this->rom.roms[11][this->u_ad_bus] >> 7) & 0b00000001;//////

		// ROM 12
		controller_bus.mdr_to_pagtbl_en = (this->rom.roms[12][this->u_ad_bus] >> 0) & 0b00000001;///
		controller_bus.force_user_ptb = (this->rom.roms[12][this->u_ad_bus] >> 1) & 0b00000001;
		// empty bit 3
		// empty bit 4
		// empty bit 5
		// empty bit 6
		controller_bus.gl_wrt = (this->rom.roms[12][this->u_ad_bus] >> 6) & 0b00000001;/////////////
		controller_bus.gh_wrt = (this->rom.roms[12][this->u_ad_bus] >> 7) & 0b00000001;/////////////

		// ROM 13
		controller_bus.imm = this->rom.roms[13][this->u_ad_bus]; ///////////////////////////////////

		// ROM 14
		// empty bit 0
		// empty bit 1
		// empty bit 2
		// empty bit 3
		// empty bit 4
		// empty bit 5
		// empty bit 6
		// empty bit 7


	}
	else {
		// ROM 0
		controller_bus.next = 0;

		// ROM 1
		controller_bus.u_offset = 0;
		controller_bus.cond_inv = 0;
		controller_bus.cond_flags_src = 0;
		controller_bus.cond_sel = 0;
		controller_bus.u_escape_0 = 0;

		// ROM 2
		controller_bus.uzf_in_src = 0;
		controller_bus.ucf_in_src = 0;
		controller_bus.usf_in_src = 0;
		controller_bus.uof_in_src = 0;
		controller_bus.ir_wrt = 0;
		controller_bus.status_wrt = 0;

		// ROM 3
		controller_bus.shift_src = 0;
		controller_bus.zbus_out_src = 0;
		controller_bus.alu_a_src = 0;

		// ROM 4
		controller_bus.alu_op = 0;
		controller_bus.alu_mode = 0;

		// ROM 5
		controller_bus.alu_cf_in_src = 0;
		controller_bus.alu_cf_in_inv = 0;
		controller_bus.zf_in_src = 0;
		controller_bus.alu_cf_out_inv = 0;
		controller_bus.cf_in_src = 0;

		// ROM 6
		controller_bus.sf_in_src = 0;
		controller_bus.of_in_src = 0;
		controller_bus.rd = 0;
		controller_bus.wr = 0;

		// ROM 7
		controller_bus.alu_b_src = 0;
		controller_bus.display_reg_load = 0;
		controller_bus.dl_wrt = 0;
		controller_bus.dh_wrt = 0;
		controller_bus.cl_wrt = 0;
		controller_bus.ch_wrt = 0;

		// ROM 8
		controller_bus.bl_wrt = 0; 
		controller_bus.bh_wrt = 0;
		controller_bus.al_wrt = 0;
		controller_bus.ah_wrt = 0;
		controller_bus.mdr_in_src = 0;
		controller_bus.mdr_out_src = 0;
		controller_bus.mdr_out_en = 0;
		controller_bus.mdrl_wrt = 0;

		// ROM 9
		controller_bus.mdrh_wrt = 0;
		controller_bus.tdrl_wrt = 0;
		controller_bus.tdrh_wrt = 0;
		controller_bus.dil_wrt = 0;
		controller_bus.dih_wrt = 0; 
		controller_bus.sil_wrt = 0;
		controller_bus.sih_wrt = 0;
		controller_bus.marl_wrt = 0;

		// ROM 10
		controller_bus.marh_wrt = 0;
		controller_bus.bpl_wrt = 0;
		controller_bus.bph_wrt = 0;
		controller_bus.pcl_wrt = 0;
		controller_bus.pch_wrt = 0;
		controller_bus.spl_wrt = 0;
		controller_bus.sph_wrt = 0;
		controller_bus.u_escape_1 = 0;

		// ROM 11
		controller_bus.u_esc_in_src = 0;
		controller_bus.int_vector_wrt = 0;
		controller_bus.mask_flags_wrt = 0;
		controller_bus.mar_in_src = 0;
		controller_bus.int_ack = 0;
		controller_bus.clear_all_ints = 0;
		controller_bus.ptb_wrt = 0;
		controller_bus.pagtbl_ram_we = 0;

		// ROM 12
		controller_bus.mdr_to_pagtbl_en = 0;
		controller_bus.force_user_ptb = 0;
		// empty bit 3
		// empty bit 4
		// empty bit 5
		// empty bit 6
		controller_bus.gl_wrt = 0;
		controller_bus.gh_wrt = 0;

		// ROM 13
		controller_bus.imm = 0;

		// ROM 14
		// empty bit 0
		// empty bit 1
		// empty bit 2
		// empty bit 3
		// empty bit 4
		// empty bit 5
		// empty bit 6
		// empty bit 7


	}
}

