#include<stdio.h>

#include "sol1_microcode.h"
#include "utils.h"


void SOL1_MICROCODE::init() {

	this->u_zf = 0x00;
	this->u_cf = 0x00;
	this->u_sf = 0x00;
	this->u_of = 0x00;
	this->u_escape = 0x00;


	this->u_esc_in_src = 0x00;
	this->u_esc = 0x00;

	this->u_offset = 0b00000000;
	this->uzf_in_src = 0x00;
	this->ucf_in_src = 0x00;
	this->usf_in_src = 0x00;
	this->uof_in_src = 0x00;

	this->U_FLAGS.set(0x00);
	this->u_adder_b = 0x00;
	this->u_ad_bus = 0x00;
	this->u_adder = 0x00;

	this->IR.set(0x00);

	this->U_ADDRESSl.set(0x00);
	this->U_ADDRESSh.set(0x00);


	this->mccycle.next = 0x00;

	this->mccycle.cond_inv = 0x00;
	this->mccycle.cond_flags_src = 0x00;
	this->mccycle.cond_sel = 0x00;


	this->mccycle.ir_wrt = 0x00;

	this->mccycle.shift_src = 0x00;
	this->mccycle.zbus_out_src = 0x00;



	this->mccycle.zf_in_src = 0x00;
	this->mccycle.cf_in_src = 0x00;
	this->mccycle.sf_in_src = 0x00;
	this->mccycle.of_in_src = 0x00;

	this->mccycle.rd = 0x00;
	this->mccycle.wr = 0x00;


	this->mccycle.display_reg_load = 0x00;
	this->mccycle.dl_wrt = 0x00;
	this->mccycle.dh_wrt = 0x00;
	this->mccycle.cl_wrt = 0x00;
	this->mccycle.ch_wrt = 0x00;
	this->mccycle.bl_wrt = 0x00;
	this->mccycle.bh_wrt = 0x00;
	this->mccycle.al_wrt = 0x00;
	this->mccycle.ah_wrt = 0x00;
	this->mccycle.mdr_in_src = 0x00;
	this->mccycle.mdr_out_src = 0x00;
	this->mccycle.mdr_out_en = 0x00;
	this->mccycle.mdrl_wrt = 0x00;
	this->mccycle.mdrh_wrt = 0x00;
	this->mccycle.tdrl_wrt = 0x00;
	this->mccycle.tdrh_wrt = 0x00;
	this->mccycle.dil_wrt = 0x00;
	this->mccycle.dih_wrt = 0x00;
	this->mccycle.sil_wrt = 0x00;
	this->mccycle.sih_wrt = 0x00;
	this->mccycle.marl_wrt = 0x00;
	this->mccycle.marh_wrt = 0x00;
	this->mccycle.bpl_wrt = 0x00;
	this->mccycle.bph_wrt = 0x00;
	this->mccycle.pcl_wrt = 0x00;
	this->mccycle.pch_wrt = 0x00;
	this->mccycle.spl_wrt = 0x00;
	this->mccycle.sph_wrt = 0x00;



	this->mccycle.mar_in_src = 0x00;


	this->mccycle.ptb_wrt = 0x00;
	this->mccycle.pagtbl_ram_we = 0x00;
	this->mccycle.mdr_to_pagtbl_en = 0x00;
	this->mccycle.force_user_ptb = 0x00;

	this->mccycle.gl_wrt = 0x00;
	this->mccycle.gh_wrt = 0x00;
	this->mccycle.imm = 0x00;


	this->mccycle.sspl_wrt = 0x01; ////////?????????????????
	this->mccycle.ssph_wrt = 0x01; ////////?????????????????


	//?????????????
	this->mccycle.memory_io = 0x00; // bus_mem_io //?????????????????
	this->mccycle.page_present = 0x00; ////////?????????????????
	this->mccycle.page_writable = 0x00; ////////?????????????????
	//?????????????
	//this->mccycle.buffer_rd = 0x00;
	//this->mccycle.buffer_wr = 0x00;
	//this->mccycle.buffer_mem_io = 0x00;
	//?????????????
	//?????????????


	// CPU FLAGS
	//this->mccycle.MSW_ZF = 0x0; ////////?????????????????
	//this->mccycle.MSW_CF = 0x00; ////////?????????????????
	//this->mccycle.MSW_SF = 0x00; ////////?????????????????
	//this->mccycle.MSW_OF = 0x00; ////////?????????????????
	//this->mccycle.MSW_12 = 0x00; ////////?????????????????
	//this->mccycle.MSW_13 = 0x00; ////////?????????????????
	//this->mccycle.MSW_14 = 0x00; ////////?????????????????
	//this->mccycle.MSW_15 = 0x00; ////////?????????????????

	// STATUS FLAGS
	//this->mccycle.MSW_DMA_ACK = 0x00; ////////?????????????????
	//this->mccycle.MSW_INTERRUPT_ENABLE = 0x00; ////////?????????????????
	//this->mccycle.MSW_CPU_MODE = 0x00; ////////?????????????????
	//this->mccycle.MSW_PAGING_EN = 0x0; ////////?????????????????
	//this->mccycle.MSW_HALT = 0x00; ////////?????????????????
	//this->mccycle.MSW_DISPLAY_REG_LOAD = 0x00; ////////?????????????????
	//this->mccycle.MSW_14 = 0x00; ////////?????????????????
	//this->mccycle.MSW_DIR = 0x00; ////////?????????????????

	//this->mccycle.mswh_wrt = 0x00; // flags
	this->mccycle.status_wrt = 0x00; //mswl_wrt // status (flags de controle)



	//
	this->mccycle.clear_all_ints = 0x00;
	this->mccycle.int_vector = 0x00; ////////?????????????????
	//this->mccycle.int_mask = 0x00; ////////?????????????????
	//this->mccycle.int_masks_wrt = 0x00; ////////?????????????????
	this->mccycle.mask_flags_wrt = 0x00;
	this->mccycle.int_status = 0x00; ////////?????????????????
	this->mccycle.int_vector_wrt = 0x00;
	this->mccycle.int_ack = 0x00;
	this->mccycle.int_request = 0x00; ////////?????????????????
	this->mccycle.int_req = 0xFF;
	//



	this->mccycle.dma_req = 0x00; ////////????????????????? CLOCK DE DMA


	this->mccycle.wait = 0x00; ////////?????????????????
	this->mccycle.ext_input = 0x00; ////////?????????????????

	this->mccycle.final_condition = 0x00;


	this->mccycle.panel_regsel = 0x00;

}




SOL1_DWORD SOL1_MICROCODE::u_adder_refresh(SOL1_BYTE typ, SOL1_BYTE final_condition) {

	//if type = branch, and condition = false, then next = +1

	//Typ1 Typ0		Desc
	//0    0		offset
	//0    1		branch
	//1    0		pre-fetch
	//1    1		post_fetch

	SOL1_BYTE u_offset6 = get_byte_bit(this->u_offset, 6);

	if (typ == 0b00000001 && !check_byte_bit(final_condition, 0))
		this->u_adder_b = 0x01;
	else
		this->u_adder_b = this->u_offset |
		set_word_bit(u_offset6, 7) |
		set_word_bit(u_offset6, 8) |
		set_word_bit(u_offset6, 9) |
		set_word_bit(u_offset6, 10) |
		set_word_bit(u_offset6, 11) |
		set_word_bit(u_offset6, 12) |
		set_word_bit(u_offset6, 13);

	this->u_adder = (this->u_ad_bus & 0b0011111111111111) + this->u_adder_b;



	return this->u_adder;
}

void SOL1_MICROCODE::display_u_adder(SOL1_BYTE typ, SOL1_BYTE final_condition) {

	printf("* next(typ): "); print_nibble_bin(typ);
	printf(" | ");
	printf(" u_offset: "); print_byte_bin(this->u_offset);
	printf(" | ");
	printf("Final Condition : "); print_nibble_bin(final_condition);
	printf("\n");

	printf("* A(u_ad): ");  print_word_bin_nibbles(this->u_ad_bus);
	printf("\n");
	printf("* B: ");  print_word_bin_nibbles(this->u_adder_b);
	printf("\n");
	printf("* u_adder: ");  print_word_bin(this->u_adder);
	printf("\n");
}


void SOL1_MICROCODE::u_flags(sol1_alu_8bit& alu, SOL1_BYTE z_bus, SOL1_BYTE uflags_debug)
{

	SOL1_BYTE inZF = 0x00;
	SOL1_BYTE inCF = 0x00;
	SOL1_BYTE inSF = 0x00;
	SOL1_BYTE inOF = 0x00;

	switch (this->uzf_in_src) {
	case 0x00:
		inZF = get_byte_bit(this->u_zf, 0);
		break;
	case 0x01:
		inZF = get_byte_bit(alu.alu_zf, 0);
		break;
	case 0x02:
		inZF = get_byte_bit(this->u_zf, 0) & get_byte_bit(alu.alu_zf, 0);
		break;
	}

	switch (this->ucf_in_src) {
	case 0x00:
		inCF = get_byte_bit(this->u_cf, 0);
		break;
	case 0x01:
		inCF = get_byte_bit(alu.alu_final_cf, 0);
		break;
	case 0x02:
		inCF = get_byte_bit(alu.alu_output, 0);
		break;
	case 0x03:
		inCF = get_byte_bit(alu.alu_output, 7);
		break;
	}

	if (this->usf_in_src == 0x00)
		inSF = get_byte_bit(this->u_sf, 0);
	else
		inSF = get_byte_bit(z_bus, 7);

	if (this->uof_in_src == 0x00)
		inOF = get_byte_bit(this->u_of, 0);
	else
		inOF = get_byte_bit(alu.alu_of, 0);


	this->u_zf = inZF;
	this->u_cf = inCF;
	this->u_sf = inSF;
	this->u_of = inOF;


	if (this->u_esc_in_src != 0x00)
		this->u_esc = this->mccycle.imm & 0b00000011;

	if (uflags_debug) {
		printf("***** U_FLAGS\n");
		display_u_flags();
	}
}


void SOL1_MICROCODE::display_u_flags() {
	printf("* FLAGS: "); print_byte_bin(this->U_FLAGS.value()); printf(" [");

	if (this->u_zf != 0x00) printf("Z"); else printf(" ");
	if (this->u_cf != 0x00) printf("C"); else printf(" ");
	if (this->u_sf != 0x00) printf("S"); else printf(" ");
	if (this->u_of != 0x00) printf("O"); else printf(" ");
	printf("]");

	printf(" | u_zf_in_src:%02x", this->uzf_in_src);
	printf(" | u_cf_in_src:%02x", this->ucf_in_src);
	printf(" | u_sf_in_src:%02x", this->usf_in_src);
	printf(" | u_of_in_src:%02x", this->uof_in_src);
	printf(" |  u_esc_in_src:%02x", this->u_esc_in_src);
	printf("\n\n");
}

void SOL1_MICROCODE::display_u_flags_lite() {
	printf("* U_FLAGS: "); print_byte_bin(this->U_FLAGS.value()); printf(" [");

	if (this->u_zf != 0x00) printf("Z"); else printf(" ");
	if (this->u_cf != 0x00) printf("C"); else printf(" ");
	if (this->u_sf != 0x00) printf("S"); else printf(" ");
	if (this->u_of != 0x00) printf("O"); else printf(" ");
	printf("]");

	printf(" | u_zf_in_src:%02x", this->uzf_in_src);
	printf(" | u_cf_in_src:%02x", this->ucf_in_src);
	printf(" | u_sf_in_src:%02x", this->usf_in_src);
	printf(" | u_of_in_src:%02x", this->uof_in_src);
	printf(" |  u_esc_in_src:%02x", this->u_esc_in_src);

	printf("\n");
}



SOL1_BYTE SOL1_MICROCODE::int_pending(SOL1_REGISTERS& registers) {
	return get_byte_bit(this->mccycle.int_request, 0) & get_byte_bit(registers.MSWl.value(), MSW_INTERRUPT_ENABLE);
}

SOL1_BYTE SOL1_MICROCODE::any_interruption(SOL1_REGISTERS& registers) {
	return get_byte_bit(int_pending(registers), 0) | get_byte_bit(this->mccycle.dma_req, 0);
}

SOL1_BYTE SOL1_MICROCODE::page_table_addr_src(SOL1_REGISTERS& registers) {
	return get_byte_bit(this->mccycle.force_user_ptb, 0) ^ get_byte_bit(registers.MSWl.value(), MSW_CPU_MODE);
}


SOL1_BYTE SOL1_MICROCODE::MUX(SOL1_REGISTERS& registers) {

	SOL1_BYTE mux_A = (this->mccycle.next == 0b00000011) || (this->mccycle.next == 0b00000010 && any_interruption(registers) == 0x01);
	SOL1_BYTE mux_B = this->mccycle.next == 0b00000010;

	SOL1_BYTE mux = set_byte_bit(mux_B, 1) | set_byte_bit(mux_A, 0);

	return mux;
}


void SOL1_MICROCODE::update_final_condition(SOL1_REGISTERS& registers) {
	if (!check_byte_bit(this->mccycle.cond_sel, 3)) {
		SOL1_BYTE inZF = 0x00;
		SOL1_BYTE inCF = 0x00;
		SOL1_BYTE inSF = 0x00;
		SOL1_BYTE inOF = 0x00;

		if (this->mccycle.cond_flags_src == 0x00) {
			inZF = get_byte_bit(registers.MSWh.value(), MSW_ZF);
			inCF = get_byte_bit(registers.MSWh.value(), MSW_CF);
			inSF = get_byte_bit(registers.MSWh.value(), MSW_SF);
			inOF = get_byte_bit(registers.MSWh.value(), MSW_OF);
		}
		else {
			inZF = get_byte_bit(this->u_zf, 0);
			inCF = get_byte_bit(this->u_cf, 0);
			inSF = get_byte_bit(this->u_sf, 0);
			inOF = get_byte_bit(this->u_of, 0);
		}

		SOL1_BYTE SFneqOF = inSF ^ inOF; //XOR
		SOL1_BYTE ZForCF = inZF | inCF;
		SOL1_BYTE ZForSFneqOF = inZF | SFneqOF;

		switch (this->mccycle.cond_sel & 0b00000111) {
		case 0x00:
			this->mccycle.final_condition = get_byte_bit(this->mccycle.cond_inv, 0) ^ get_byte_bit(inZF, 0);
			break;

		case 0x01:
			this->mccycle.final_condition = get_byte_bit(this->mccycle.cond_inv, 0) ^ get_byte_bit(inCF, 0);
			break;

		case 0x02:
			this->mccycle.final_condition = get_byte_bit(this->mccycle.cond_inv, 0) ^ get_byte_bit(inSF, 0);
			break;

		case 0x03:
			this->mccycle.final_condition = get_byte_bit(this->mccycle.cond_inv, 0) ^ get_byte_bit(inOF, 0);
			break;

		case 0x04:
			this->mccycle.final_condition = get_byte_bit(this->mccycle.cond_inv, 0) ^ get_byte_bit(SFneqOF, 0);
			break;

		case 0x05:
			this->mccycle.final_condition = get_byte_bit(this->mccycle.cond_inv, 0) ^ get_byte_bit(ZForSFneqOF, 0);
			break;

		case 0x06:
			this->mccycle.final_condition = get_byte_bit(this->mccycle.cond_inv, 0) ^ get_byte_bit(ZForCF, 0);
			break;

		case 0x07:
			this->mccycle.final_condition = get_byte_bit(this->mccycle.cond_inv, 0) ^ get_byte_bit(this->mccycle.dma_req, 0);
			break;
		}
	}
	else {
		switch (this->mccycle.cond_sel & 0b00000111) {
		case 0x00:
			this->mccycle.final_condition = get_byte_bit(this->mccycle.cond_inv, 0) ^ get_byte_bit(registers.MSWl.value(), MSW_CPU_MODE);
			break;

		case 0x01:
			this->mccycle.final_condition = get_byte_bit(this->mccycle.cond_inv, 0) ^ get_byte_bit(this->mccycle.wait, 0);
			break;

		case 0x02:
			this->mccycle.final_condition = get_byte_bit(this->mccycle.cond_inv, 0) ^ int_pending(registers);
			break;

		case 0x03:
			this->mccycle.final_condition = get_byte_bit(this->mccycle.cond_inv, 0) ^ get_byte_bit(this->mccycle.ext_input, 0);
			break;

		case 0x04:
			this->mccycle.final_condition = get_byte_bit(this->mccycle.cond_inv, 0) ^ get_byte_bit(registers.MSWl.value(), MSW_DIR);
			break;

		case 0x05:
			this->mccycle.final_condition = get_byte_bit(this->mccycle.cond_inv, 0) ^ get_byte_bit(registers.MSWl.value(), MSW_DISPLAY_REG_LOAD);
			break;

		case 0x06:
			this->mccycle.final_condition = get_byte_bit(this->mccycle.cond_inv, 0) ^ 0;
			break;

		case 0x07:
			this->mccycle.final_condition = get_byte_bit(this->mccycle.cond_inv, 0) ^ 0;
			break;

		}

	}
}
void SOL1_MICROCODE::addresser(SOL1_REGISTERS& registers, SOL1_BYTE addresser_debug) {

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

	SOL1_BYTE mux = MUX(registers);


	SOL1_DWORD u_address_mux_l = 0x00;
	SOL1_DWORD u_address_mux_h = 0x00;

	if (mux == 0x00) {
		u_address_mux_l = this->u_adder & 0b00000011111111;
		u_address_mux_h = (this->u_adder & 0b11111100000000) >> 8;
	}
	else if (mux == 0x01) {
		u_address_mux_l = ((this->IR.value() & 0b00000011) << 6) | (get_byte_bit(this->u_escape, 0) << 4);
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
	this->u_ad_bus = u_address;



	if (addresser_debug) {
		printf("***** U_ADDRESSER\n");
		printf("* Next(typ): "); print_nibble_bin(this->mccycle.next);
		printf(" | Any Interruption: "); print_nibble_bin(any_interruption(registers));
		//printf(" | Mux_B: "); print_nibble_bin(mux_B);
		//printf(" | Mux_A: "); print_nibble_bin(mux_A);
		printf("\n");

		/*
		printf("* "); print_byte_bin(in01_A); printf(" | ");  print_byte_bin(in01_B); printf(" | ");  print_byte_bin(res01); printf("\n");
		printf("* "); print_byte_bin(in02_A); printf(" | ");  print_byte_bin(in02_B); printf(" | ");  print_byte_bin(res02); printf("\n");
		printf("* "); print_byte_bin(in03_A); printf(" | ");  print_byte_bin(in03_B); printf(" | ");  print_byte_bin(res03); printf("\n");
		printf("* "); print_byte_bin(in04_A); printf(" | ");  print_byte_bin(in04_B); printf(" | ");  print_byte_bin(res04); printf("\n");
		printf("* "); print_byte_bin(in05_A); printf(" | ");  print_byte_bin(in05_B); printf(" | ");  print_byte_bin(res05); printf("\n");
		printf("* "); print_byte_bin(in06_A); printf(" | ");  print_byte_bin(in06_B); printf(" | ");  print_byte_bin(res06); printf("\n");
		printf("* "); print_byte_bin(in07_A); printf(" | ");  print_byte_bin(in07_B); printf(" | ");  print_byte_bin(res07); printf("\n");
		*/
		printf("* U_ADDRESS=%04x", SOL1_REGISTERS::value(this->U_ADDRESSl, this->U_ADDRESSh));
		printf(" | ");
		printf("U_AD=%04x", this->u_ad_bus);
		printf(" | ");
		printf("Mux: "); print_nibble_bin(mux);
		printf("\n");

		printf("\n");
	}

	//return this->u_ad_bus;
}




