//
// sol1_controller_bus.h
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
#ifndef SOL1CONTROLLERBUS_H
#define SOL1CONTROLLERBUS_H

#include "config.h"

struct sol1_controller_bus {

	//ROM 01
	SOL1_BYTE next;		//TYP
	SOL1_BYTE u_offset;

	//ROM 02
	SOL1_BYTE cond_inv;
	SOL1_BYTE cond_flags_src;
	SOL1_BYTE cond_sel;
	SOL1_BYTE u_escape_0;

	//ROM 03
	SOL1_BYTE uzf_in_src;
	SOL1_BYTE ucf_in_src;
	SOL1_BYTE usf_in_src;
	SOL1_BYTE uof_in_src;
	SOL1_BYTE ir_wrt;
	SOL1_BYTE status_wrt; //mswl_wrt // status (flags de controle)

	//ROM 04
	SOL1_BYTE shift_src;
	SOL1_BYTE zbus_out_src;
	SOL1_BYTE alu_a_src;

	//ROM 05
	SOL1_BYTE alu_op;
	SOL1_BYTE alu_mode;

	//ROM 06
	SOL1_BYTE alu_cf_in_src;
	SOL1_BYTE alu_cf_in_inv;
	SOL1_BYTE zf_in_src; // ZeroFlag
	SOL1_BYTE alu_cf_out_inv;
	SOL1_BYTE cf_in_src; // Carry Flag

	//ROM 07
	SOL1_BYTE sf_in_src; // Sign Flag
	SOL1_BYTE of_in_src; // Overflow Flag
	SOL1_BYTE rd;
	SOL1_BYTE wr;

	//ROM 08
	SOL1_BYTE alu_b_src;
	SOL1_BYTE display_reg_load;
	SOL1_BYTE dl_wrt;
	SOL1_BYTE dh_wrt;
	SOL1_BYTE cl_wrt;
	SOL1_BYTE ch_wrt;

	//ROM 09
	SOL1_BYTE bl_wrt;
	SOL1_BYTE bh_wrt;
	SOL1_BYTE al_wrt;
	SOL1_BYTE ah_wrt;
	SOL1_BYTE mdr_in_src;
	SOL1_BYTE mdr_out_src;
	SOL1_BYTE mdr_out_en;
	SOL1_BYTE mdrl_wrt;

	//ROM 10
	SOL1_BYTE mdrh_wrt;
	SOL1_BYTE tdrl_wrt;
	SOL1_BYTE tdrh_wrt;
	SOL1_BYTE dil_wrt;
	SOL1_BYTE dih_wrt;
	SOL1_BYTE sil_wrt;
	SOL1_BYTE sih_wrt;
	SOL1_BYTE marl_wrt;

	//ROM 11
	SOL1_BYTE marh_wrt;
	SOL1_BYTE bpl_wrt;
	SOL1_BYTE bph_wrt;
	SOL1_BYTE pcl_wrt;
	SOL1_BYTE pch_wrt;
	SOL1_BYTE spl_wrt;
	SOL1_BYTE sph_wrt;
	SOL1_BYTE u_escape_1;

	//ROM 12
	SOL1_BYTE u_esc_in_src;
	SOL1_BYTE int_vector_wrt;
	SOL1_BYTE mask_flags_wrt;
	SOL1_BYTE mar_in_src;
	SOL1_BYTE int_ack;
	SOL1_BYTE clear_all_ints;
	SOL1_BYTE ptb_wrt;
	SOL1_BYTE pagtbl_ram_we;

	//ROM 13
	SOL1_BYTE mdr_to_pagtbl_en;
	SOL1_BYTE force_user_ptb;
	//SOL1_BYTE IC6_3;
	//SOL1_BYTE IC6_4;
	//SOL1_BYTE IC6_5;
	//SOL1_BYTE IC6_6;
	SOL1_BYTE gl_wrt;
	SOL1_BYTE gh_wrt;

	//ROM 14
	SOL1_BYTE imm;

	//ROM 15
	//SOL1_BYTE IC8_1;
	//SOL1_BYTE IC8_2;
	//SOL1_BYTE IC8_3;
	//SOL1_BYTE IC8_4;
	//SOL1_BYTE IC8_5;
	//SOL1_BYTE IC8_6;
	//SOL1_BYTE IC8_7;
	//SOL1_BYTE IC8_8;




	///////////////////////////////////////////////////////////
	SOL1_BYTE sspl_wrt;
	SOL1_BYTE ssph_wrt;

	//?????????????
	SOL1_BYTE memory_io; // bus_mem_io //?????????????????
	SOL1_BYTE page_present; ////////?????????????????
	SOL1_BYTE page_writable; ////////?????????????????
	//?????????????



	//

	SOL1_BYTE int_vector;

	SOL1_BYTE int_status;


	SOL1_BYTE int_request;
	SOL1_BYTE int_req;
	//

	SOL1_BYTE dma_req; ////////?????????????????

	SOL1_BYTE wait; ////////?????????????????
	SOL1_BYTE ext_input; ////////?????????????????

	SOL1_BYTE final_condition;

	SOL1_BYTE panel_regsel;
};

#endif