//
// baffa1_microcode.h
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
#ifndef BAFFA1MICROCODE_H
#define BAFFA1MICROCODE_H


#include "config.h"
#include "baffa1_registers.h"
#include "baffa1_rom.h"
#include "baffa1_alu.h"
#include "hw_tty.h"

#include "baffa1_controller_bus.h"

class BAFFA1_MICROCODE {

public:


	/*
	BAFFA1_BYTE u_zf;
	BAFFA1_BYTE u_cf;
	BAFFA1_BYTE u_sf;
	BAFFA1_BYTE u_of;
	BAFFA1_BYTE u_esc;
	BAFFA1_REGISTER_8BIT U_FLAGS;
	
	BAFFA1_BYTE u_zf;
	BAFFA1_BYTE u_cf;
	BAFFA1_BYTE u_sf;
	BAFFA1_BYTE u_of;
	BAFFA1_BYTE u_escape;

	BAFFA1_BYTE u_esc_in_src;
	

	BAFFA1_BYTE u_offset;
	BAFFA1_BYTE uzf_in_src;
	BAFFA1_BYTE ucf_in_src;
	BAFFA1_BYTE usf_in_src;
	BAFFA1_BYTE uof_in_src;
	*/

	//BAFFA1_REGISTER_8BIT U_FLAGS; 
	BAFFA1_DWORD u_adder_b;
	BAFFA1_DWORD u_ad_bus; BAFFA1_DWORD old_u_ad_bus;
	BAFFA1_DWORD u_adder;
	/////

	baffa1_controller_bus controller_bus;

	BAFFA1_REGISTER_8BIT IR;

	BAFFA1_REGISTER_8BIT U_ADDRESSl;
	BAFFA1_REGISTER_8BIT U_ADDRESSh;

	BAFFA1_ROM rom;


	void init(HW_TTY& hw_tty);
	
	BAFFA1_DWORD u_adder_refresh(BAFFA1_BYTE typ, BAFFA1_BYTE final_condition, BAFFA1_CONFIG& config, HW_TTY& hw_tty);

	BAFFA1_BYTE page_table_addr_src(BAFFA1_BYTE reg_status_value);
	BAFFA1_BYTE int_pending(BAFFA1_BYTE reg_status_value);
	BAFFA1_BYTE any_interruption(BAFFA1_BYTE reg_status_value);

	BAFFA1_BYTE MUX(BAFFA1_BYTE reg_status_value);

	
	void sequencer_update(BAFFA1_BYTE reg_status_value, BAFFA1_CONFIG& config, HW_TTY& hw_tty);


private:

	void display_u_adder(BAFFA1_BYTE typ, BAFFA1_BYTE final_condition, HW_TTY& hw_tty);

	void addresser(BAFFA1_BYTE reg_status_value, BAFFA1_CONFIG& config, HW_TTY& hw_tty);

	void load_microcode_from_rom();
};
#endif

