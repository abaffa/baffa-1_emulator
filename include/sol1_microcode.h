#ifndef SOL1MICROCODE_H
#define SOL1MICROCODE_H


#include "config.h"
#include "sol1_registers.h"
#include "sol1_alu.h"
#include "hw_tty.h"


struct sol1_mccycle {
	SOL1_BYTE next;

	SOL1_BYTE cond_inv;
	SOL1_BYTE cond_flags_src;
	SOL1_BYTE cond_sel;


	SOL1_BYTE ir_wrt;

	SOL1_BYTE shift_src;
	SOL1_BYTE zbus_out_src;

	SOL1_BYTE zf_in_src; // ZeroFlag
	SOL1_BYTE cf_in_src; // Carry Flag
	SOL1_BYTE sf_in_src; // Sign Flag
	SOL1_BYTE of_in_src; // Overflow Flag

	SOL1_BYTE rd;
	SOL1_BYTE wr;


	SOL1_BYTE display_reg_load;
	SOL1_BYTE dl_wrt;
	SOL1_BYTE dh_wrt;
	SOL1_BYTE cl_wrt;
	SOL1_BYTE ch_wrt;
	SOL1_BYTE bl_wrt;
	SOL1_BYTE bh_wrt;
	SOL1_BYTE al_wrt;
	SOL1_BYTE ah_wrt;
	SOL1_BYTE mdr_in_src;
	SOL1_BYTE mdr_out_src;
	SOL1_BYTE mdr_out_en;
	SOL1_BYTE mdrl_wrt;
	SOL1_BYTE mdrh_wrt;
	SOL1_BYTE tdrl_wrt;
	SOL1_BYTE tdrh_wrt;
	SOL1_BYTE dil_wrt;
	SOL1_BYTE dih_wrt;
	SOL1_BYTE sil_wrt;
	SOL1_BYTE sih_wrt;
	SOL1_BYTE marl_wrt;
	SOL1_BYTE marh_wrt;
	SOL1_BYTE bpl_wrt;
	SOL1_BYTE bph_wrt;
	SOL1_BYTE pcl_wrt;
	SOL1_BYTE pch_wrt;
	SOL1_BYTE spl_wrt;
	SOL1_BYTE sph_wrt;



	SOL1_BYTE mar_in_src;


	SOL1_BYTE ptb_wrt;
	SOL1_BYTE pagtbl_ram_we;
	SOL1_BYTE mdr_to_pagtbl_en;
	SOL1_BYTE force_user_ptb;

	SOL1_BYTE gl_wrt;
	SOL1_BYTE gh_wrt;
	SOL1_BYTE imm;


	SOL1_BYTE sspl_wrt = 0x01; ////////?????????????????
	SOL1_BYTE ssph_wrt = 0x01; ////////?????????????????




	//?????????????
	SOL1_BYTE memory_io; // bus_mem_io //?????????????????
	SOL1_BYTE page_present; ////////?????????????????
	SOL1_BYTE page_writable; ////////?????????????????
	//?????????????


	SOL1_BYTE status_wrt; //mswl_wrt // status (flags de controle)

	//
	SOL1_BYTE clear_all_ints;
	SOL1_BYTE int_vector; ////////?????????????????
	//SOL1_BYTE int_mask; ////////?????????????????
	//SOL1_BYTE int_masks_wrt; ////////?????????????????
	SOL1_BYTE mask_flags_wrt;
	SOL1_BYTE int_status; ////////?????????????????
	SOL1_BYTE int_vector_wrt;
	SOL1_BYTE int_ack;
	SOL1_BYTE int_request; ////////?????????????????
	SOL1_BYTE int_req;
	//

	SOL1_BYTE dma_req; ////////?????????????????



	SOL1_BYTE wait; ////////?????????????????
	SOL1_BYTE ext_input; ////////?????????????????

	SOL1_BYTE final_condition;


	SOL1_BYTE panel_regsel;
};


class SOL1_MICROCODE {

public:

	//SOL1_COMPUTER& sol1_computer;

	SOL1_BYTE u_zf;
	SOL1_BYTE u_cf;
	SOL1_BYTE u_sf;
	SOL1_BYTE u_of;
	SOL1_BYTE u_escape;

	SOL1_BYTE u_esc_in_src;
	SOL1_BYTE u_esc;

	SOL1_BYTE u_offset;
	SOL1_BYTE uzf_in_src;
	SOL1_BYTE ucf_in_src;
	SOL1_BYTE usf_in_src;
	SOL1_BYTE uof_in_src;

	SOL1_REGISTER_8BIT U_FLAGS;
	SOL1_DWORD u_adder_b;
	SOL1_DWORD u_ad_bus; SOL1_DWORD old_u_ad_bus;
	SOL1_DWORD u_adder;
	/////

	sol1_mccycle mccycle;

	SOL1_REGISTER_8BIT IR;

	SOL1_REGISTER_8BIT U_ADDRESSl;
	SOL1_REGISTER_8BIT U_ADDRESSh;




	void init();


	SOL1_DWORD u_adder_refresh(SOL1_BYTE typ, SOL1_BYTE final_condition);
	void display_u_adder(SOL1_BYTE typ, SOL1_BYTE final_condition, HW_TTY& hw_tty);

	void u_flags(sol1_alu_8bit& alu, SOL1_BYTE z_bus, SOL1_BYTE uflags_debug, HW_TTY& hw_tty);

	void display_u_flags(HW_TTY& hw_tty);
	void display_u_flags_lite(HW_TTY& hw_tty);


	SOL1_BYTE int_pending(SOL1_REGISTERS& registers);
	SOL1_BYTE any_interruption(SOL1_REGISTERS& registers);
	SOL1_BYTE page_table_addr_src(SOL1_REGISTERS& registers);

	SOL1_BYTE MUX(SOL1_REGISTERS& registers);

	void update_final_condition(SOL1_REGISTERS& registers);
	void addresser(SOL1_REGISTERS& registers, SOL1_BYTE addresser_debug, HW_TTY& hw_tty);

	

};
#endif

