//
// sol1_microcode.h
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
#ifndef SOL1MICROCODE_H
#define SOL1MICROCODE_H


#include "config.h"
#include "sol1_registers.h"
#include "sol1_rom.h"
#include "sol1_alu.h"
#include "hw_tty.h"

#include "sol1_controller_bus.h"

class SOL1_MICROCODE {

public:

	SOL1_BYTE u_zf;
	SOL1_BYTE u_cf;
	SOL1_BYTE u_sf;
	SOL1_BYTE u_of;
	SOL1_BYTE u_esc;

	/*
	SOL1_BYTE u_zf;
	SOL1_BYTE u_cf;
	SOL1_BYTE u_sf;
	SOL1_BYTE u_of;
	SOL1_BYTE u_escape;

	SOL1_BYTE u_esc_in_src;
	

	SOL1_BYTE u_offset;
	SOL1_BYTE uzf_in_src;
	SOL1_BYTE ucf_in_src;
	SOL1_BYTE usf_in_src;
	SOL1_BYTE uof_in_src;
	*/

	SOL1_REGISTER_8BIT U_FLAGS;
	SOL1_DWORD u_adder_b;
	SOL1_DWORD u_ad_bus; SOL1_DWORD old_u_ad_bus;
	SOL1_DWORD u_adder;
	/////

	sol1_controller_bus controller_bus;

	SOL1_REGISTER_8BIT IR;

	SOL1_REGISTER_8BIT U_ADDRESSl;
	SOL1_REGISTER_8BIT U_ADDRESSh;

	SOL1_ROM rom;


	void init(HW_TTY& hw_tty);
	
	SOL1_DWORD u_adder_refresh(SOL1_BYTE typ, SOL1_BYTE final_condition, SOL1_CONFIG& config, HW_TTY& hw_tty);
	void u_flags_refresh(SOL1_BYTE reg_status_value, SOL1_BYTE reg_flags_value, struct sol1_alu_bus *alu_bus, SOL1_CONFIG& config, HW_TTY& hw_tty);
	
	SOL1_BYTE page_table_addr_src(SOL1_BYTE reg_status_value);
	SOL1_BYTE int_pending(SOL1_BYTE reg_status_value);
	SOL1_BYTE any_interruption(SOL1_BYTE reg_status_value);

	SOL1_BYTE MUX(SOL1_BYTE reg_status_value);

	
	void sequencer_update(SOL1_BYTE reg_status_value, SOL1_CONFIG& config, HW_TTY& hw_tty);


private:
	void update_final_condition(SOL1_BYTE reg_status_value, SOL1_BYTE reg_flags_value);

	void display_u_adder(SOL1_BYTE typ, SOL1_BYTE final_condition, HW_TTY& hw_tty);
	void display_u_flags(HW_TTY& hw_tty);
	void display_u_flags_lite(HW_TTY& hw_tty);

	void addresser(SOL1_BYTE reg_status_value, SOL1_CONFIG& config, HW_TTY& hw_tty);

	void load_microcode_from_rom();
};
#endif

