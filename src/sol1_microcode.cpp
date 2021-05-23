#include "sol1_microcode.h"
#include "sol1_cpu.h"
#include "ttl74series.h"
#include "utils.h"


void sol1_microcode_init(struct sol1_microcode *sol1_microcode) {

	sol1_microcode->u_zf = 0x00;
	sol1_microcode->u_cf = 0x00;
	sol1_microcode->u_sf = 0x00;
	sol1_microcode->u_of = 0x00;
	sol1_microcode->u_esc = 0x00;
	sol1_microcode->u_esc_in_src = 0x00;

	sol1_microcode->u_offset = 0b00000000;
	sol1_microcode->uzf_in_src = 0x00;
	sol1_microcode->ucf_in_src = 0x00;
	sol1_microcode->usf_in_src = 0x00;
	sol1_microcode->uof_in_src = 0x00;

	sol1_microcode->U_FLAGS = sol1_register_8bit_create();
	sol1_microcode->u_adder_b = 0x00;
	sol1_microcode->u_ad_bus = 0x00;
	sol1_microcode->u_adder = 0x00;

	sol1_microcode->IR = sol1_register_8bit_create();

	sol1_microcode->U_ADDRESSl = sol1_register_8bit_create();
	sol1_microcode->U_ADDRESSh = sol1_register_8bit_create();


	sol1_microcode->mccycle.next = 0x00;

	sol1_microcode->mccycle.cond_inv = 0x00;
	sol1_microcode->mccycle.cond_flags_src = 0x00;
	sol1_microcode->mccycle.cond_sel = 0x00;


	sol1_microcode->mccycle.ir_wrt = 0x00;

	sol1_microcode->mccycle.shift_src = 0x00;
	sol1_microcode->mccycle.zbus_out_src = 0x00;



	sol1_microcode->mccycle.zf_in_src = 0x00;
	sol1_microcode->mccycle.cf_in_src = 0x00;
	sol1_microcode->mccycle.sf_in_src = 0x00;
	sol1_microcode->mccycle.of_in_src = 0x00;

	sol1_microcode->mccycle.rd = 0x00;
	sol1_microcode->mccycle.wr = 0x00;


	sol1_microcode->mccycle.display_reg_load = 0x00;
	sol1_microcode->mccycle.dl_wrt = 0x00;
	sol1_microcode->mccycle.dh_wrt = 0x00;
	sol1_microcode->mccycle.cl_wrt = 0x00;
	sol1_microcode->mccycle.ch_wrt = 0x00;
	sol1_microcode->mccycle.bl_wrt = 0x00;
	sol1_microcode->mccycle.bh_wrt = 0x00;
	sol1_microcode->mccycle.al_wrt = 0x00;
	sol1_microcode->mccycle.ah_wrt = 0x00;
	sol1_microcode->mccycle.mdr_in_src = 0x00;
	sol1_microcode->mccycle.mdr_out_src = 0x00;
	sol1_microcode->mccycle.mdr_out_en = 0x00;
	sol1_microcode->mccycle.mdrl_wrt = 0x00;
	sol1_microcode->mccycle.mdrh_wrt = 0x00;
	sol1_microcode->mccycle.tdrl_wrt = 0x00;
	sol1_microcode->mccycle.tdrh_wrt = 0x00;
	sol1_microcode->mccycle.dil_wrt = 0x00;
	sol1_microcode->mccycle.dih_wrt = 0x00;
	sol1_microcode->mccycle.sil_wrt = 0x00;
	sol1_microcode->mccycle.sih_wrt = 0x00;
	sol1_microcode->mccycle.marl_wrt = 0x00;
	sol1_microcode->mccycle.marh_wrt = 0x00;
	sol1_microcode->mccycle.bpl_wrt = 0x00;
	sol1_microcode->mccycle.bph_wrt = 0x00;
	sol1_microcode->mccycle.pcl_wrt = 0x00;
	sol1_microcode->mccycle.pch_wrt = 0x00;
	sol1_microcode->mccycle.spl_wrt = 0x00;
	sol1_microcode->mccycle.sph_wrt = 0x00;



	sol1_microcode->mccycle.mar_in_src = 0x00;


	sol1_microcode->mccycle.ptb_wrt = 0x00;
	sol1_microcode->mccycle.pagtbl_ram_we = 0x00;
	sol1_microcode->mccycle.mdr_to_pagtbl_en = 0x00;
	sol1_microcode->mccycle.force_user_ptb = 0x00;

	sol1_microcode->mccycle.gl_wrt = 0x00;
	sol1_microcode->mccycle.gh_wrt = 0x00;
	sol1_microcode->mccycle.imm = 0x00;


	sol1_microcode->mccycle.sspl_wrt = 0x01; ////////?????????????????
	sol1_microcode->mccycle.ssph_wrt = 0x01; ////////?????????????????

	sol1_microcode->mccycle.page_table_addr_src = 0x0; ////////?????????????????



	//?????????????
	sol1_microcode->mccycle.memory_io = 0x00; // bus_mem_io //?????????????????
	sol1_microcode->mccycle.page_present = 0x00; ////////?????????????????
	sol1_microcode->mccycle.page_writable = 0x00; ////////?????????????????
	//?????????????
	//sol1_microcode->mccycle.buffer_rd = 0x00;
	//sol1_microcode->mccycle.buffer_wr = 0x00;
	//sol1_microcode->mccycle.buffer_mem_io = 0x00;
	//?????????????
	//?????????????


	// CPU FLAGS
	//sol1_microcode->mccycle.MSW_ZF = 0x0; ////////?????????????????
	//sol1_microcode->mccycle.MSW_CF = 0x00; ////////?????????????????
	//sol1_microcode->mccycle.MSW_SF = 0x00; ////////?????????????????
	//sol1_microcode->mccycle.MSW_OF = 0x00; ////////?????????????????
	//sol1_microcode->mccycle.MSW_12 = 0x00; ////////?????????????????
	//sol1_microcode->mccycle.MSW_13 = 0x00; ////////?????????????????
	//sol1_microcode->mccycle.MSW_14 = 0x00; ////////?????????????????
	//sol1_microcode->mccycle.MSW_15 = 0x00; ////////?????????????????

	// STATUS FLAGS
	//sol1_microcode->mccycle.MSW_DMA_ACK = 0x00; ////////?????????????????
	//sol1_microcode->mccycle.MSW_INTERRUPT_ENABLE = 0x00; ////////?????????????????
	//sol1_microcode->mccycle.MSW_CPU_MODE = 0x00; ////////?????????????????
	//sol1_microcode->mccycle.MSW_PAGING_EN = 0x0; ////////?????????????????
	//sol1_microcode->mccycle.MSW_HALT = 0x00; ////////?????????????????
	//sol1_microcode->mccycle.MSW_DISPLAY_REG_LOAD = 0x00; ////////?????????????????
	//sol1_microcode->mccycle.MSW_14 = 0x00; ////////?????????????????
	//sol1_microcode->mccycle.MSW_DIR = 0x00; ////////?????????????????

	//sol1_microcode->mccycle.mswh_wrt = 0x00; // flags
	sol1_microcode->mccycle.status_wrt = 0x00; //mswl_wrt // status (flags de controle)



	//
	sol1_microcode->mccycle.clear_all_ints = 0x00;
	sol1_microcode->mccycle.int_vector = 0x00; ////////?????????????????
	sol1_microcode->mccycle.int_mask = 0x00; ////////?????????????????
	//sol1_microcode->mccycle.int_masks_wrt = 0x00; ////////?????????????????
	sol1_microcode->mccycle.mask_flags_wrt = 0x00;
	sol1_microcode->mccycle.int_status = 0x00; ////////?????????????????
	sol1_microcode->mccycle.int_vector_wrt = 0x00;
	sol1_microcode->mccycle.int_ack = 0x00;
	sol1_microcode->mccycle.int_request = 0x00; ////////?????????????????
	//


	sol1_microcode->mccycle.int_pending = 0x00; //?? int_request + MSW_INTERRUPT_ENABLE
	sol1_microcode->mccycle.dma_req = 0x00; ////////????????????????? CLOCK DE DMA
	sol1_microcode->mccycle.any_interruption = 0x00; //?? int_endind "+" dma_req


	sol1_microcode->mccycle.wait = 0x00; ////////?????????????????
	sol1_microcode->mccycle.ext_input = 0x00; ////////?????????????????

	sol1_microcode->mccycle.final_condition = 0x00;


	sol1_microcode->mccycle.panel_regsel = 0x00;

}


void sol1_microcode_destroy(struct sol1_microcode *sol1_microcode) {
	free(sol1_microcode->U_FLAGS);
	free(sol1_microcode->U_ADDRESSl);
	free(sol1_microcode->U_ADDRESSh);
}





SOL1_DWORD sol1_u_adder(struct sol1_microcode *sol1_microcode,
	SOL1_BYTE typ,
	SOL1_BYTE final_condition) {

	/*
	SOL1_BYTE typ_0 = (typ & 0b00000001);
	SOL1_BYTE typ_1 = get_byte_bit(typ, 1);

	SOL1_BYTE inAB = IC_74LS10( //IC145A
		typ_0,
		IC_74LS04(typ_1),			//IC43A
		IC_74LS04(final_condition)	//IC43B
	);


	SOL1_BYTE in01_A = 0x1;
	SOL1_BYTE in01_B = 0b00001111 & u_offset;

	SOL1_BYTE in02_A = 0x00;
	SOL1_BYTE in02_B = (0b00000111 & (u_offset >> 4)) | (u_offset6 << 3);

	//upper bits are a copy of u_offset6. this means this offset
	//is a signed number from - 64 to 63
	SOL1_BYTE in03_A = 0x00;
	SOL1_BYTE in03_B = (u_offset6) ||
		(u_offset6 << 1) ||
		(u_offset6 << 2) ||
		(u_offset6 << 3);

	SOL1_BYTE in04_A = 0x00;
	SOL1_BYTE in04_B = (u_offset6) ||
		(u_offset6 << 1);

	SOL1_BYTE inOEAB = 0b00000001 & inAB;

	SOL1_DWORD res =
		IC_74LS257(						//IC49
			in01_A,
			in01_B,
			inOEAB) |
			(IC_74LS257(				//IC55
				in02_A,
				in02_B,
				inOEAB) << 4) |
				(IC_74LS257(			//IC63
					in03_A,
					in03_B,
					inOEAB) << 8) |
					(IC_74LS257(		//IC100
						in04_A,
						in04_B,
						inOEAB) << 12);

	if (DEBUG_UADDER) {
		printf("* inOEAB: "); print_nibble_bin(inOEAB);
		printf("\n");

		printf("* in01 A: "); print_nibble_bin(in01_A);
		printf(" | B: "); print_nibble_bin(in01_B);
		printf("\n");

		printf("* in02 A: "); print_nibble_bin(in02_A);
		printf(" | B: "); print_nibble_bin(in02_B);
		printf("\n");

		printf("* in03 A: "); print_nibble_bin(in03_A);
		printf(" | B: "); print_nibble_bin(in03_B);
		printf("\n");

		printf("* in04 A: "); print_nibble_bin(in04_A);
		printf(" | B: "); print_nibble_bin(in04_B);
		printf("\n");
		printf("* u_ad: ");  print_word_bin_nibbles(res);
		printf("\n");
	}
	*/
	//if type = branch, and condition = false, then next = +1

	//Typ1 Typ0		Desc
	//0    0		offset
	//0    1		branch
	//1    0		pre-fetch
	//1    1		post_fetch

	SOL1_BYTE u_offset6 = get_byte_bit(sol1_microcode->u_offset, 6);

	if (typ == 0b00000001 && get_byte_bit(final_condition, 0) == 0x00)
		sol1_microcode->u_adder_b = 0x01;
	else
		sol1_microcode->u_adder_b = sol1_microcode->u_offset |
		set_byte_bit(u_offset6, 7) |
		set_byte_bit(u_offset6, 8) |
		set_byte_bit(u_offset6, 9) |
		set_byte_bit(u_offset6, 10) |
		set_byte_bit(u_offset6, 11) |
		set_byte_bit(u_offset6, 12) |
		set_byte_bit(u_offset6, 13);

	sol1_microcode->u_adder = (sol1_microcode->u_ad_bus & 0b0011111111111111) + sol1_microcode->u_adder_b;



	return sol1_microcode->u_adder;
}
void sol1_microcode_display_u_adder(struct sol1_microcode *sol1_microcode, SOL1_BYTE typ, SOL1_BYTE final_condition) {

	printf("* next(typ): "); print_nibble_bin(typ);
	printf(" | ");
	printf(" u_offset: "); print_byte_bin(sol1_microcode->u_offset);
	printf(" | ");
	printf("Final Condition : "); print_nibble_bin(final_condition);
	printf("\n");

	printf("* A(u_ad): ");  print_word_bin_nibbles(sol1_microcode->u_ad_bus);
	printf("\n");
	printf("* B: ");  print_word_bin_nibbles(sol1_microcode->u_adder_b);
	printf("\n");
	printf("* u_adder: ");  print_word_bin(sol1_microcode->u_adder);
	printf("\n");
}


void sol1_u_flags(sol1_alu_8bit *alu, struct sol1_microcode *mc, SOL1_BYTE imm, SOL1_BYTE z_bus)
{
	
	//ALU_0 IS WHAT THE CARRY FLAG BECOMES
			//AFTER A SHIFT RIGHT OPERATION.
			//
			//WARNING: THIS IS NOT FROM THE ZBUS!!THIS
			//IS BEFORE THE ALU'S OUTPUT IS SHIFTED!!
	/*
	SOL1_BYTE inFlags00 = IC_74LS257( //IC256
		mc->u_esc & 0b00000011,
		imm & 0b00000011, // IMMY?
		mc->u_esc_in_src); //U_ESC_SRC???

	SOL1_BYTE inFlags01 = IC_74LS153( //IC237
		get_byte_bit(mc->u_zf, 0) |
		(get_byte_bit(alu->alu_zf, 0) << 1) |
		(get_byte_bit(IC_74LS08(alu->alu_zf, mc->u_zf), 0) << 2),
		0x00,
		mc->uzf_in_src,
		0x00);


	SOL1_BYTE inFlags02 = IC_74LS153( //IC270
		get_byte_bit(mc->u_cf, 0) |
		(get_byte_bit(alu->alu_final_cf, 0) << 1) |
		(get_byte_bit(alu->alu_output, 0) << 2) |
		(get_byte_bit(alu->alu_output, 7) << 3),
		0x00,
		mc->ucf_in_src,
		0x00);

	SOL1_BYTE inFlags03 =
		IC_74LS257(get_byte_bit(mc->u_sf, 0), //IC121
			get_byte_bit(z_bus, 7),
			mc->usf_in_src);

	SOL1_BYTE inFlags04 = IC_74LS257(get_byte_bit(mc->u_of, 0), //IC127
		get_byte_bit(alu->alu_of, 0),
		mc->uof_in_src);

	SOL1_BYTE inFlags_res = IC_74LS273(mc->U_FLAGS, //IC231
		get_byte_bit(inFlags00, 0) |
		(get_byte_bit(inFlags01, 0) << 1) |
		(get_byte_bit(inFlags02, 0) << 2) |
		(get_byte_bit(inFlags03, 0) << 3) |
		((inFlags00 & 0b00000011) << 4),
		0x1, 0x1); //clk, ~rst
	mc->u_zf = get_byte_bit(inFlags_res, 0);
	mc->u_cf = get_byte_bit(inFlags_res, 1);
	mc->u_sf = get_byte_bit(inFlags_res, 2);
	mc->u_of = get_byte_bit(inFlags_res, 3);
	mc->u_esc = (inFlags_res & 0b00110000) >> 4;
*/


	SOL1_BYTE inZF = 0x00;
	SOL1_BYTE inCF = 0x00;
	SOL1_BYTE inSF = 0x00;
	SOL1_BYTE inOF = 0x00;

	switch (mc->uzf_in_src) {
	case 0x00:
		inZF = get_byte_bit(mc->u_zf, 0);
		break;
	case 0x01:
		inZF = get_byte_bit(alu->alu_zf, 0);
		break;
	case 0x02:
		inZF = get_byte_bit(mc->u_zf, 0) == get_byte_bit(alu->alu_zf, 0);
		break;
	}

	switch (mc->ucf_in_src) {
	case 0x00:
		inCF = get_byte_bit(mc->u_cf, 0);
		break;
	case 0x01:
		inCF = get_byte_bit(alu->alu_final_cf, 0);
		break;
	case 0x02:
		inCF = get_byte_bit(alu->alu_output, 0);
		break;
	case 0x03:
		inCF = get_byte_bit(alu->alu_output, 7);
		break;
	}

	if (mc->usf_in_src == 0x00)
		inSF = get_byte_bit(mc->u_sf, 0);
	else
		inSF = get_byte_bit(z_bus, 7);

	if (mc->uof_in_src == 0x00)
		inOF = get_byte_bit(mc->u_of, 0);
	else
		inOF = get_byte_bit(alu->alu_of, 0);


	mc->u_zf = inZF;
	mc->u_cf = inCF;
	mc->u_sf = inSF;
	mc->u_of = inOF;

	if (mc->u_esc_in_src != 0x00)
		mc->u_esc = imm & 0b00000011;
}


void sol1_microcode_display_u_flags(struct sol1_microcode *mc) {
	printf("* U_FLAGS="); print_byte_bin(sol1_register_8bit_value(mc->U_FLAGS));
	printf(" | ");
	printf("u_esc="); print_byte_bin(mc->u_esc);
	if (mc->u_esc_in_src != 0x00) printf(" u_esc_in_src");
	printf("\n");

	printf("* u_flags:\n");

	if (mc->u_zf != 0x00) printf("u_zf ");
	if (mc->u_cf != 0x00) printf("u_cf ");
	if (mc->u_sf != 0x00) printf("u_sf ");
	if (mc->u_of != 0x00) printf("u_of ");
	printf("\n");

	if (mc->uzf_in_src != 0x00) printf("u_zf_in_src ");
	if (mc->ucf_in_src != 0x00) printf("u_cf_in_src ");
	if (mc->usf_in_src != 0x00) printf("u_sf_in_src ");
	if (mc->uof_in_src != 0x00) printf("u_of_in_src ");
	printf("\n");
}






SOL1_BYTE MUX(struct sol1_microcode *sol1_microcode) {

	SOL1_BYTE mux_A = (sol1_microcode->mccycle.next == 0b00000011) || (sol1_microcode->mccycle.next == 0b00000010 && get_byte_bit(sol1_microcode->mccycle.any_interruption, 0) == 0x01);
	SOL1_BYTE mux_B = sol1_microcode->mccycle.next == 0b00000010;

	SOL1_BYTE mux = set_byte_bit(mux_B, 1) | set_byte_bit(mux_A, 0);

	return mux;
}


void UADDRESSER( struct sol1_cpu *sol1_cpu) {

	////////////////////////////////////////////////////////////////////////////
	//IC15 //IC59 //IC153 //IC167 //IC61 //IC12 //IC341 //IC175

	if (get_byte_bit(sol1_cpu->microcode.mccycle.cond_sel, 3) == 0x00) {
		SOL1_BYTE inZF = 0x00;
		SOL1_BYTE inCF = 0x00;
		SOL1_BYTE inSF = 0x00;
		SOL1_BYTE inOF = 0x00;

		if (sol1_cpu->microcode.mccycle.cond_flags_src == 0x00) {
			inZF = get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWh), MSW_ZF);
			inCF = get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWh), MSW_CF);
			inSF = get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWh), MSW_SF);
			inOF = get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWh), MSW_OF);
		}
		else {
			inZF = sol1_cpu->microcode.u_zf;
			inCF = sol1_cpu->microcode.u_cf;
			inSF = sol1_cpu->microcode.u_sf;
			inOF = sol1_cpu->microcode.u_of;
		}

		SOL1_BYTE SFneqOF = inSF != inOF; //XOR
		SOL1_BYTE ZForCF = inZF | inCF;
		SOL1_BYTE ZForSFneqOF = inZF | SFneqOF;

		switch (sol1_cpu->microcode.mccycle.cond_sel & 0b00000111) {
		case 0x00:
			sol1_cpu->microcode.mccycle.final_condition = get_byte_bit(sol1_cpu->microcode.mccycle.cond_inv, 0) != get_byte_bit(inZF, 0);
			break;

		case 0x01:
			sol1_cpu->microcode.mccycle.final_condition = get_byte_bit(sol1_cpu->microcode.mccycle.cond_inv, 0) != get_byte_bit(inCF, 0);
			break;

		case 0x02:
			sol1_cpu->microcode.mccycle.final_condition = get_byte_bit(sol1_cpu->microcode.mccycle.cond_inv, 0) != get_byte_bit(inSF, 0);
			break;

		case 0x03:
			sol1_cpu->microcode.mccycle.final_condition = get_byte_bit(sol1_cpu->microcode.mccycle.cond_inv, 0) != get_byte_bit(inOF, 0);
			break;

		case 0x04:
			sol1_cpu->microcode.mccycle.final_condition = get_byte_bit(sol1_cpu->microcode.mccycle.cond_inv, 0) != get_byte_bit(SFneqOF, 0);
			break;

		case 0x05:
			sol1_cpu->microcode.mccycle.final_condition = get_byte_bit(sol1_cpu->microcode.mccycle.cond_inv, 0) != get_byte_bit(ZForCF, 0);
			break;

		case 0x06:
			sol1_cpu->microcode.mccycle.final_condition = get_byte_bit(sol1_cpu->microcode.mccycle.cond_inv, 0) != get_byte_bit(ZForSFneqOF, 0);
			break;

		case 0x07:
			sol1_cpu->microcode.mccycle.final_condition = get_byte_bit(sol1_cpu->microcode.mccycle.cond_inv, 0) != get_byte_bit(sol1_cpu->microcode.mccycle.dma_req, 0);
			break;
		}
	}
	else {
		switch (sol1_cpu->microcode.mccycle.cond_sel & 0b00000111) {
		case 0x00:
			sol1_cpu->microcode.mccycle.final_condition = get_byte_bit(sol1_cpu->microcode.mccycle.cond_inv, 0) != get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWl), MSW_CPU_MODE);
			break;

		case 0x01:
			sol1_cpu->microcode.mccycle.final_condition = get_byte_bit(sol1_cpu->microcode.mccycle.cond_inv, 0) != get_byte_bit(sol1_cpu->microcode.mccycle.wait, 0);
			break;

		case 0x02:
			sol1_cpu->microcode.mccycle.final_condition = get_byte_bit(sol1_cpu->microcode.mccycle.cond_inv, 0) != get_byte_bit(sol1_cpu->microcode.mccycle.int_pending, 0);
			break;

		case 0x03:
			sol1_cpu->microcode.mccycle.final_condition = get_byte_bit(sol1_cpu->microcode.mccycle.cond_inv, 0) != get_byte_bit(sol1_cpu->microcode.mccycle.ext_input, 0);
			break;

		case 0x04:
			sol1_cpu->microcode.mccycle.final_condition = get_byte_bit(sol1_cpu->microcode.mccycle.cond_inv, 0) != get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWl), MSW_DIR);
			break;

		case 0x05:
			sol1_cpu->microcode.mccycle.final_condition = get_byte_bit(sol1_cpu->microcode.mccycle.cond_inv, 0) != get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWl), MSW_DISPLAY_REG_LOAD);
			break;
		}

	}


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

	SOL1_BYTE mux = MUX(&sol1_cpu->microcode);


	SOL1_DWORD u_address_mux_l = 0x00;
	SOL1_DWORD u_address_mux_h = 0x00;

	if (mux == 0x00) {
		u_address_mux_l = sol1_cpu->microcode.u_adder & 0b00000011111111;
		u_address_mux_h = (sol1_cpu->microcode.u_adder & 0b11111100000000) >> 8;
	}
	else if (mux == 0x01) {
		u_address_mux_l = (sol1_register_8bit_value(sol1_cpu->microcode.IR) & 0b00000011) << 6;
		u_address_mux_h = (sol1_register_8bit_value(sol1_cpu->microcode.IR) & 0b11111100) >> 2;
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
	sol1_registers_set(sol1_cpu->microcode.U_ADDRESSl, sol1_cpu->microcode.U_ADDRESSh, u_address);

	sol1_cpu->microcode.old_u_ad_bus = sol1_cpu->microcode.u_ad_bus;
	sol1_cpu->microcode.u_ad_bus = u_address;



	if (DEBUG_UADDRESSER) {
		printf("***** U_ADDRESSER\n");
		printf("* Next(typ): "); print_nibble_bin(sol1_cpu->microcode.mccycle.next);
		printf(" | Any Interruption: "); print_nibble_bin(sol1_cpu->microcode.mccycle.any_interruption);
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
		printf("* U_ADDRESS=%04x", sol1_registers_value(sol1_cpu->microcode.U_ADDRESSl, sol1_cpu->microcode.U_ADDRESSh));
		printf(" | ");
		printf("U_AD=%04x", sol1_cpu->microcode.u_ad_bus);
		printf(" | ");
		printf("Mux: "); print_nibble_bin(mux);
		printf("\n");

		printf("\n");
	}

	//return sol1_cpu->microcode.u_ad_bus;
}




void mc_seq_update(sol1_cpu *sol1_cpu,
	sol1_alu_8bit *alu) {

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

	//CLOCK LOW
	UADDRESSER(sol1_cpu);


	/*
	int i = 0;
	int j = 0;
	for (i = 0; i < 15; i++) {
		//microcode = microcode | (sol1_cpu->rom.roms[i][u_ad_bus] << (8 * i));

		if (DEBUG_MICROCODE && sol1_cpu->rom.roms[i][sol1_cpu->microcode.u_ad_bus] != 0x00)
			for (j = 0; j < 8; j++)
				if ((sol1_cpu->rom.roms[i][sol1_cpu->microcode.u_ad_bus] >> j) & 0x1 != 0)
					printf("%s, ", SOL1_ROM_CONTROL_LIST[(i * 8) + j]);
	}
	if (DEBUG_MICROCODE) printf("\n");
	SOL1_BYTE opcode = u_ad_bus;
	int cycle = 0;
	int p = opcode * SOL1_ROM_CYCLES_PER_INSTR + cycle; <<<< u_ad_bus
	*/

	////////////////////////////////////////////////////////////////////////////

	load_microcode_from_rom(sol1_cpu, alu);

	if (DEBUG_MICROCODE) {
		printf("***** MICROCODE\n");
		//printf("U-ADDRESS: ");  print_word_bin(sol1_cpu->microcode.u_ad_bus); printf("\n");		
		//printf("OPCODE: %02x (cycle %02x)\n", (sol1_cpu->microcode.u_ad_bus / 64), (sol1_cpu->microcode.u_ad_bus % 64));
		//printf("microcode: \n");
		printf("* U_FLAGS="); print_byte_bin(sol1_register_8bit_value(sol1_cpu->microcode.U_FLAGS)); printf("\n");
		sol1_rom_display_current_cycles_desc(&sol1_cpu->rom, (sol1_cpu->microcode.u_ad_bus / 64), (sol1_cpu->microcode.u_ad_bus % 64));
		printf("\n");
	}

}

