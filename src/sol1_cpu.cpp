#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include <time.h>

#include "sol1_cpu.h"
#include "utils.h"

#define DEBUG_OPCODE 1

void sol1_cpu_init(struct sol1_cpu* sol1_cpu)
{
    memset(sol1_cpu, 0, sizeof(struct sol1_cpu));
	sol1_memory_init(&sol1_cpu->memory);
	sol1_registers_init(&sol1_cpu->registers);
	sol1_microcode_init(&sol1_cpu->microcode);
	sol1_rom_init(&sol1_cpu->rom);
	sol1_cpu_reset(sol1_cpu);
}

void sol1_cpu_reset(struct sol1_cpu *sol1_cpu)
{
	//DATA REGISTERS
	sol1_registers_reset(sol1_cpu->registers.Al, sol1_cpu->registers.Ah); // AX (16bit) Accumulator	(Ah/Al)
	sol1_registers_reset(sol1_cpu->registers.Bl, sol1_cpu->registers.Bh); // BX (16bit) Base		(Bh/Bl)
	sol1_registers_reset(sol1_cpu->registers.Cl, sol1_cpu->registers.Ch); // CX (16bit) Counter		(Ch/Cl)
	sol1_registers_reset(sol1_cpu->registers.Dl, sol1_cpu->registers.Dh); // DX (16bit) Data		(Dh/Dl)
	sol1_registers_reset(sol1_cpu->registers.Gl, sol1_cpu->registers.Gh); // GX (16bit)	Gh/Gl	General Register(For scratch)

	//Pointer Registers
	sol1_registers_reset(sol1_cpu->registers.BPl, sol1_cpu->registers.BPh); // BP (16bit) Base Pointer  (Used to manage stack frames)
	sol1_registers_reset(sol1_cpu->registers.SPl, sol1_cpu->registers.SPh); // SP (16bit) Stack Pointer
	sol1_registers_reset(sol1_cpu->registers.SSPl, sol1_cpu->registers.SSPh); // SSP (16bit) Supervisor Stack Pointer

	//Index Registers
	sol1_registers_reset(sol1_cpu->registers.SIl, sol1_cpu->registers.SIh); // SI (16bit) Source index (Source address for string operations)
	sol1_registers_reset(sol1_cpu->registers.DIl, sol1_cpu->registers.DIh); // DI (16bit) Destination Index (Destination address for string operations)
	sol1_registers_reset(sol1_cpu->registers.PCl, sol1_cpu->registers.PCh); // PC (16bit) Program Counter

	sol1_registers_reset(sol1_cpu->registers.TDRl, sol1_cpu->registers.TDRh); // TDR (16bit) Temporary Data Register
	sol1_register_8bit_reset(sol1_cpu->registers.PTB);  // PTB (8bit) = Page table base

	sol1_registers_reset(sol1_cpu->registers.MSWl, sol1_cpu->registers.MSWh); // 

	sol1_registers_reset(sol1_cpu->memory.MARl, sol1_cpu->memory.MARh); // 
	sol1_registers_reset(sol1_cpu->memory.MDRl, sol1_cpu->memory.MDRh); // 

	//JumpZ80(z80, z80->registers.PC);

	sol1_cpu->BKPT = 0xFFFF; // Breakpoint
}

void sol1_cpu_display_registers(struct sol1_cpu *sol1_cpu) {
	SOL1_BYTE opcode = sol1_cpu->memory.memory[sol1_registers_value(sol1_cpu->registers.PCl, sol1_cpu->registers.PCh)];
	printf(" DATA REGISTERS\n");
	printf(" *A=%04x", sol1_registers_value(sol1_cpu->registers.Al, sol1_cpu->registers.Ah));
	printf(" | ");
	printf("B=%04x", sol1_registers_value(sol1_cpu->registers.Bl, sol1_cpu->registers.Bh));
	printf(" | ");
	printf("C=%04x", sol1_registers_value(sol1_cpu->registers.Cl, sol1_cpu->registers.Ch));
	printf(" | ");
	printf("D=%04x", sol1_registers_value(sol1_cpu->registers.Dl, sol1_cpu->registers.Dh));
	printf(" | ");
	printf("G=%04x", sol1_registers_value(sol1_cpu->registers.Gl, sol1_cpu->registers.Gh));
	printf("\n\n");

	printf(" POINTER REGISTERS\n");
	printf(" *BP=%04x", sol1_registers_value(sol1_cpu->registers.BPl, sol1_cpu->registers.BPh));
	printf(" | ");
	printf("SP=%04x", sol1_registers_value(sol1_cpu->registers.SPl, sol1_cpu->registers.SPh));
	printf(" | ");
	printf("SSP=%04x", sol1_registers_value(sol1_cpu->registers.SSPl, sol1_cpu->registers.SSPh));
	printf("\n\n");

	printf(" INDEX REGISTERS\n");
	printf(" *SI=%04x", sol1_registers_value(sol1_cpu->registers.SIl, sol1_cpu->registers.SIh));
	printf(" | ");
	printf("DI=%04x", sol1_registers_value(sol1_cpu->registers.DIl, sol1_cpu->registers.DIh));
	printf(" | ");
	printf("PC=%04x", sol1_registers_value(sol1_cpu->registers.PCl, sol1_cpu->registers.PCh));
	printf(" | ");
	printf("@PC=%02x:%s", opcode, &sol1_cpu->rom.rom_desc[0x400000 + (opcode * 256)]);
	printf("\n\n");

	printf(" SPECIAL REGISTERS\n");
	printf(" *TDR=%04x", sol1_registers_value(sol1_cpu->registers.TDRl, sol1_cpu->registers.TDRh));
	printf(" | ");
	printf("PTB=%02x", sol1_register_8bit_value(sol1_cpu->registers.PTB));
	printf("\n");
	printf(" *FLAGS="); print_byte_bin(sol1_register_8bit_value(sol1_cpu->registers.MSWh));
	printf(" | ");
	printf("STATUS="); print_byte_bin(sol1_register_8bit_value(sol1_cpu->registers.MSWl));
	printf("\n\n");

	printf(" MEMORY REGISTERS\n");
	printf(" * MAR=%04x", sol1_registers_value(sol1_cpu->memory.MARl, sol1_cpu->memory.MARh));
	printf(" | ");
	printf("MDR=%04x", sol1_registers_value(sol1_cpu->memory.MDRl, sol1_cpu->memory.MDRh));
	printf("\n");
}

void sol1_cpu_display_registers_lite(struct sol1_cpu *sol1_cpu) {
	SOL1_BYTE opcode = sol1_cpu->memory.memory[sol1_registers_value(sol1_cpu->registers.PCl, sol1_cpu->registers.PCh)];

	printf("*   A=%04x", sol1_registers_value(sol1_cpu->registers.Al, sol1_cpu->registers.Ah));
	printf(" | ");
	printf("  B=%04x", sol1_registers_value(sol1_cpu->registers.Bl, sol1_cpu->registers.Bh));
	printf(" | ");
	printf("  C=%04x", sol1_registers_value(sol1_cpu->registers.Cl, sol1_cpu->registers.Ch));
	printf(" | ");
	printf("  D=%04x", sol1_registers_value(sol1_cpu->registers.Dl, sol1_cpu->registers.Dh));
	printf(" | ");
	printf("  G=%04x", sol1_registers_value(sol1_cpu->registers.Gl, sol1_cpu->registers.Gh));
	printf("\n");

	printf("*  SI=%04x", sol1_registers_value(sol1_cpu->registers.SIl, sol1_cpu->registers.SIh));
	printf(" | ");
	printf(" DI=%04x", sol1_registers_value(sol1_cpu->registers.DIl, sol1_cpu->registers.DIh));
	printf(" | ");
	printf(" PC=%04x", sol1_registers_value(sol1_cpu->registers.PCl, sol1_cpu->registers.PCh));
	printf(" | ");
	printf("@PC=%02x:%s", opcode, &sol1_cpu->rom.rom_desc[0x400000 + (opcode * 256)]);
	printf("\n");

	printf("* MAR=%04x", sol1_registers_value(sol1_cpu->memory.MARl, sol1_cpu->memory.MARh));
	printf(" | ");
	printf("MDR=%04x", sol1_registers_value(sol1_cpu->memory.MDRl, sol1_cpu->memory.MDRh));
	printf(" | ");
	printf("TDR=%04x", sol1_registers_value(sol1_cpu->registers.TDRl, sol1_cpu->registers.TDRh));
	printf(" | ");
	printf("PTB=%02x", sol1_register_8bit_value(sol1_cpu->registers.PTB));
	printf("\n");

	printf("*  BP=%04x", sol1_registers_value(sol1_cpu->registers.BPl, sol1_cpu->registers.BPh));
	printf(" | ");
	printf(" SP=%04x", sol1_registers_value(sol1_cpu->registers.SPl, sol1_cpu->registers.SPh));
	printf(" | ");
	printf("SSP=%04x", sol1_registers_value(sol1_cpu->registers.SSPl, sol1_cpu->registers.SSPh));
	printf("\n");

	printf("* FLAGS="); print_byte_bin(sol1_register_8bit_value(sol1_cpu->registers.MSWh));
	printf("   | ");
	printf("STATUS="); print_byte_bin(sol1_register_8bit_value(sol1_cpu->registers.MSWl));
	printf("\n");
}




void sol1_cpu_memory_display(struct sol1_cpu *sol1_cpu)
{

	int i = 0, j = 0;
	SOL1_DWORD PC = sol1_registers_value(sol1_cpu->registers.PCl, sol1_cpu->registers.PCh);

	if (sol1_cpu->memory.debug_manual_offset == 0x00) {
		if (0 + sol1_cpu->memory.debug_mem_offset > PC || PC >= 256 + sol1_cpu->memory.debug_mem_offset)
			sol1_cpu->memory.debug_mem_offset = (PC / 0x10) * 0x10;
	}

	printf("\n        ");

	for (i = 0; i < 16; i++)
		printf("%02x ", i);

	printf("\n\n %04x ", sol1_cpu->memory.debug_mem_offset);

	for (i = 0 + sol1_cpu->memory.debug_mem_offset; i < 256 + sol1_cpu->memory.debug_mem_offset; i++) {
		if (i % 16 == 0)
			if (PC == i)
				printf(" *");
			else
				printf("  ");
		if (PC == i || PC - 1 == i)
			printf("%02x*", sol1_cpu->memory.memory[i]);
		else
			printf("%02x ", sol1_cpu->memory.memory[i]);

		if ((i + 1) % 16 == 0 && i <= 255 + sol1_cpu->memory.debug_mem_offset) {
			printf("  |");
			for (j = (i + 1) - 16; j < (i + 1); j++) {
				if (sol1_cpu->memory.memory[j] < 0x20)
					printf(".");
				else
					printf("%c", sol1_cpu->memory.memory[j]);
			}
			printf("|");

			if (i < 255 + sol1_cpu->memory.debug_mem_offset)
				printf("\n %04x ", i + 1);
			else
				printf("\n");

		}
	}
}


void mswh_flags_desc(struct sol1_cpu* sol1_cpu) {

	SOL1_BYTE b = sol1_register_8bit_value(sol1_cpu->registers.MSWh);
	if (get_byte_bit(b, MSW_ZF) != 0x00)
		printf("zf ");
	if (get_byte_bit(b, MSW_CF) != 0x00)
		printf("cf ");
	if (get_byte_bit(b, MSW_SF) != 0x00)
		printf("sf ");
	if (get_byte_bit(b, MSW_OF) != 0x00)
		printf("of ");
}




void mswl_status_desc(struct sol1_cpu* sol1_cpu) {

	SOL1_BYTE b = sol1_register_8bit_value(sol1_cpu->registers.MSWl);
	if (get_byte_bit(b, MSW_DMA_ACK) != 0x00)
		printf("dma_ack ");
	if (get_byte_bit(b, MSW_INTERRUPT_ENABLE) != 0x00)
		printf("interrupt_enable ");
	if (get_byte_bit(b, MSW_CPU_MODE) != 0x00)
		printf("cpu_mode ");
	if (get_byte_bit(b, MSW_PAGING_EN) != 0x00)
		printf("paging_en ");
	if (get_byte_bit(b, MSW_HALT) != 0x00)
		printf("halt ");
	if (get_byte_bit(b, MSW_DISPLAY_REG_LOAD) != 0x00)
		printf("display_reg_load ");
	if (get_byte_bit(b, MSW_DIR) != 0x00)
		printf("dir ");
}




void load_microcode_from_rom(struct sol1_cpu *sol1_cpu,
	sol1_alu_8bit *alu) {

	sol1_cpu->microcode.mccycle.next = sol1_cpu->rom.roms[0][sol1_cpu->microcode.u_ad_bus] & 0b00000011;///////////////////////

	sol1_cpu->microcode.u_offset = ((sol1_cpu->rom.roms[1][sol1_cpu->microcode.u_ad_bus] & 0b00000001) << 6) |/////////
		((sol1_cpu->rom.roms[0][sol1_cpu->microcode.u_ad_bus] >> 2) & 0b00111111);////////////////////

	sol1_cpu->microcode.mccycle.cond_inv = (sol1_cpu->rom.roms[1][sol1_cpu->microcode.u_ad_bus] >> 1) & 0b00000001;
	sol1_cpu->microcode.mccycle.cond_flags_src = (sol1_cpu->rom.roms[1][sol1_cpu->microcode.u_ad_bus] >> 2) & 0b00000001;
	sol1_cpu->microcode.mccycle.cond_sel = (sol1_cpu->rom.roms[1][sol1_cpu->microcode.u_ad_bus] >> 3) & 0b00001111;
	sol1_cpu->microcode.u_esc_in_src = sol1_cpu->rom.roms[1][sol1_cpu->microcode.u_ad_bus] >> 7 & 0b00000001; //ESCAPE

	sol1_cpu->microcode.uzf_in_src = sol1_cpu->rom.roms[2][sol1_cpu->microcode.u_ad_bus] & 0b00000011;/////////////////
	sol1_cpu->microcode.ucf_in_src = (sol1_cpu->rom.roms[2][sol1_cpu->microcode.u_ad_bus] >> 2) & 0b00000011;//////////
	sol1_cpu->microcode.usf_in_src = (sol1_cpu->rom.roms[2][sol1_cpu->microcode.u_ad_bus] >> 4) & 0b00000001;//////////
	sol1_cpu->microcode.uof_in_src = (sol1_cpu->rom.roms[2][sol1_cpu->microcode.u_ad_bus] >> 5) & 0b00000001;//////////
	sol1_cpu->microcode.mccycle.ir_wrt = (sol1_cpu->rom.roms[2][sol1_cpu->microcode.u_ad_bus] >> 6) & 0b00000001;/////////////////
	sol1_cpu->microcode.mccycle.status_wrt = (sol1_cpu->rom.roms[2][sol1_cpu->microcode.u_ad_bus] >> 7) & 0b00000001;/////////////

	sol1_cpu->microcode.mccycle.shift_src = sol1_cpu->rom.roms[3][sol1_cpu->microcode.u_ad_bus] & 0b00000111;/////////////////////
	sol1_cpu->microcode.mccycle.zbus_out_src = (sol1_cpu->rom.roms[3][sol1_cpu->microcode.u_ad_bus] >> 3) & 0b00000011;///////////

	alu->alu_a_src = ((sol1_cpu->rom.roms[3][sol1_cpu->microcode.u_ad_bus] >> 5) & 0b00000111) | ///////
		((sol1_cpu->rom.roms[4][sol1_cpu->microcode.u_ad_bus] & 0b00000111) << 3);/////////////////////

	alu->alu_op = (sol1_cpu->rom.roms[4][sol1_cpu->microcode.u_ad_bus] >> 3) & 0b00001111;//////////////
	alu->alu_mode = (sol1_cpu->rom.roms[4][sol1_cpu->microcode.u_ad_bus] >> 7) & 0b00000001;////////////

	alu->alu_cf_in_src = (sol1_cpu->rom.roms[5][sol1_cpu->microcode.u_ad_bus]) & 0b00000011;////////////
	alu->alu_cf_in_inv = (sol1_cpu->rom.roms[5][sol1_cpu->microcode.u_ad_bus] >> 2) & 0b00000001;///////
	sol1_cpu->microcode.mccycle.zf_in_src = (sol1_cpu->rom.roms[5][sol1_cpu->microcode.u_ad_bus] >> 3) & 0b00000011;///////////////


	alu->alu_cf_out_inv = (sol1_cpu->rom.roms[5][sol1_cpu->microcode.u_ad_bus] >> 5) & 0b00000001;//////
	sol1_cpu->microcode.mccycle.cf_in_src = ((sol1_cpu->rom.roms[5][sol1_cpu->microcode.u_ad_bus] >> 6) & 0b00000011) |////////////
		((sol1_cpu->rom.roms[6][sol1_cpu->microcode.u_ad_bus] & 0b00000001) << 2);/////////////////////

	sol1_cpu->microcode.mccycle.sf_in_src = (sol1_cpu->rom.roms[6][sol1_cpu->microcode.u_ad_bus] >> 1) & 0b00000011;///////////////

	sol1_cpu->microcode.mccycle.of_in_src = (sol1_cpu->rom.roms[6][sol1_cpu->microcode.u_ad_bus] >> 3) & 0b00000111;///////////////

	sol1_cpu->microcode.mccycle.rd = (sol1_cpu->rom.roms[6][sol1_cpu->microcode.u_ad_bus] >> 6) & 0b00000001;//////////////////////
	sol1_cpu->microcode.mccycle.wr = (sol1_cpu->rom.roms[6][sol1_cpu->microcode.u_ad_bus] >> 7) & 0b00000001;//////////////////////

	alu->alu_b_src = sol1_cpu->rom.roms[7][sol1_cpu->microcode.u_ad_bus] & 0b00000111;//////////////////

	sol1_cpu->microcode.mccycle.display_reg_load = (sol1_cpu->rom.roms[7][sol1_cpu->microcode.u_ad_bus] >> 3) & 0b00000001;////
	sol1_cpu->microcode.mccycle.dl_wrt = (sol1_cpu->rom.roms[7][sol1_cpu->microcode.u_ad_bus] >> 4) & 0b00000001;//////////////
	sol1_cpu->microcode.mccycle.dh_wrt = (sol1_cpu->rom.roms[7][sol1_cpu->microcode.u_ad_bus] >> 5) & 0b00000001;//////////////
	sol1_cpu->microcode.mccycle.cl_wrt = (sol1_cpu->rom.roms[7][sol1_cpu->microcode.u_ad_bus] >> 6) & 0b00000001;//////////////
	sol1_cpu->microcode.mccycle.ch_wrt = (sol1_cpu->rom.roms[7][sol1_cpu->microcode.u_ad_bus] >> 7) & 0b00000001;//////////////
	sol1_cpu->microcode.mccycle.bl_wrt = (sol1_cpu->rom.roms[8][sol1_cpu->microcode.u_ad_bus] >> 0) & 0b00000001;//////////////
	sol1_cpu->microcode.mccycle.bh_wrt = (sol1_cpu->rom.roms[8][sol1_cpu->microcode.u_ad_bus] >> 1) & 0b00000001;//////////////
	sol1_cpu->microcode.mccycle.al_wrt = (sol1_cpu->rom.roms[8][sol1_cpu->microcode.u_ad_bus] >> 2) & 0b00000001;//////////////
	sol1_cpu->microcode.mccycle.ah_wrt = (sol1_cpu->rom.roms[8][sol1_cpu->microcode.u_ad_bus] >> 3) & 0b00000001;//////////////
	sol1_cpu->microcode.mccycle.mdr_in_src = (sol1_cpu->rom.roms[8][sol1_cpu->microcode.u_ad_bus] >> 4) & 0b00000001;//////////
	sol1_cpu->microcode.mccycle.mdr_out_src = (sol1_cpu->rom.roms[8][sol1_cpu->microcode.u_ad_bus] >> 5) & 0b00000001;/////////
	sol1_cpu->microcode.mccycle.mdr_out_en = (sol1_cpu->rom.roms[8][sol1_cpu->microcode.u_ad_bus] >> 6) & 0b00000001;//////////
	sol1_cpu->microcode.mccycle.mdrl_wrt = (sol1_cpu->rom.roms[8][sol1_cpu->microcode.u_ad_bus] >> 7) & 0b00000001;////////////
	sol1_cpu->microcode.mccycle.mdrh_wrt = (sol1_cpu->rom.roms[9][sol1_cpu->microcode.u_ad_bus] >> 0) & 0b00000001;////////////
	sol1_cpu->microcode.mccycle.tdrl_wrt = (sol1_cpu->rom.roms[9][sol1_cpu->microcode.u_ad_bus] >> 1) & 0b00000001;////////////
	sol1_cpu->microcode.mccycle.tdrh_wrt = (sol1_cpu->rom.roms[9][sol1_cpu->microcode.u_ad_bus] >> 2) & 0b00000001;////////////
	sol1_cpu->microcode.mccycle.dil_wrt = (sol1_cpu->rom.roms[9][sol1_cpu->microcode.u_ad_bus] >> 3) & 0b00000001;/////////////
	sol1_cpu->microcode.mccycle.dih_wrt = (sol1_cpu->rom.roms[9][sol1_cpu->microcode.u_ad_bus] >> 4) & 0b00000001;/////////////
	sol1_cpu->microcode.mccycle.sil_wrt = (sol1_cpu->rom.roms[9][sol1_cpu->microcode.u_ad_bus] >> 5) & 0b00000001;/////////////
	sol1_cpu->microcode.mccycle.sih_wrt = (sol1_cpu->rom.roms[9][sol1_cpu->microcode.u_ad_bus] >> 6) & 0b00000001;/////////////
	sol1_cpu->microcode.mccycle.marl_wrt = (sol1_cpu->rom.roms[9][sol1_cpu->microcode.u_ad_bus] >> 7) & 0b00000001;////////////
	sol1_cpu->microcode.mccycle.marh_wrt = (sol1_cpu->rom.roms[10][sol1_cpu->microcode.u_ad_bus] >> 0) & 0b00000001;///////////
	sol1_cpu->microcode.mccycle.bpl_wrt = (sol1_cpu->rom.roms[10][sol1_cpu->microcode.u_ad_bus] >> 1) & 0b00000001;////////////
	sol1_cpu->microcode.mccycle.bph_wrt = (sol1_cpu->rom.roms[10][sol1_cpu->microcode.u_ad_bus] >> 2) & 0b00000001;////////////
	sol1_cpu->microcode.mccycle.pcl_wrt = (sol1_cpu->rom.roms[10][sol1_cpu->microcode.u_ad_bus] >> 3) & 0b00000001;////////////
	sol1_cpu->microcode.mccycle.pch_wrt = (sol1_cpu->rom.roms[10][sol1_cpu->microcode.u_ad_bus] >> 4) & 0b00000001;////////////
	sol1_cpu->microcode.mccycle.spl_wrt = (sol1_cpu->rom.roms[10][sol1_cpu->microcode.u_ad_bus] >> 5) & 0b00000001;////////////
	sol1_cpu->microcode.mccycle.sph_wrt = (sol1_cpu->rom.roms[10][sol1_cpu->microcode.u_ad_bus] >> 6) & 0b00000001;////////////

	sol1_cpu->microcode.mccycle.int_vector_wrt = (sol1_cpu->rom.roms[11][sol1_cpu->microcode.u_ad_bus] >> 1) & 0b00000001;/////
	sol1_cpu->microcode.mccycle.mask_flags_wrt = (sol1_cpu->rom.roms[11][sol1_cpu->microcode.u_ad_bus] >> 2) & 0b00000001;/////
	sol1_cpu->microcode.mccycle.mar_in_src = (sol1_cpu->rom.roms[11][sol1_cpu->microcode.u_ad_bus] >> 3) & 0b00000001;/////////
	sol1_cpu->microcode.mccycle.int_ack = (sol1_cpu->rom.roms[11][sol1_cpu->microcode.u_ad_bus] >> 4) & 0b00000001;
	sol1_cpu->microcode.mccycle.clear_all_ints = (sol1_cpu->rom.roms[11][sol1_cpu->microcode.u_ad_bus] >> 5) & 0b00000001;
	sol1_cpu->microcode.mccycle.ptb_wrt = (sol1_cpu->rom.roms[11][sol1_cpu->microcode.u_ad_bus] >> 6) & 0b00000001;////////////
	sol1_cpu->microcode.mccycle.pagtbl_ram_we = (sol1_cpu->rom.roms[11][sol1_cpu->microcode.u_ad_bus] >> 7) & 0b00000001;//////
	sol1_cpu->microcode.mccycle.mdr_to_pagtbl_en = (sol1_cpu->rom.roms[12][sol1_cpu->microcode.u_ad_bus] >> 0) & 0b00000001;///
	sol1_cpu->microcode.mccycle.force_user_ptb = (sol1_cpu->rom.roms[12][sol1_cpu->microcode.u_ad_bus] >> 1) & 0b00000001;

	sol1_cpu->microcode.mccycle.gl_wrt = (sol1_cpu->rom.roms[12][sol1_cpu->microcode.u_ad_bus] >> 6) & 0b00000001;/////////////
	sol1_cpu->microcode.mccycle.gh_wrt = (sol1_cpu->rom.roms[12][sol1_cpu->microcode.u_ad_bus] >> 7) & 0b00000001;/////////////

	sol1_cpu->microcode.mccycle.imm = sol1_cpu->rom.roms[13][sol1_cpu->microcode.u_ad_bus]; ///////////////////////////////////
}




SOL1_BYTE refresh_MSWh_ZF(sol1_cpu *sol1_cpu, sol1_alu_8bit *alu, SOL1_BYTE z_bus, SOL1_BYTE zf_in_src) {

	SOL1_BYTE inMSWh_ZF = 0x00;

	switch (zf_in_src & 0b00000011) {
	case 0x00:
		inMSWh_ZF = get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWh), MSW_ZF);
		break;

	case 0x01:
		inMSWh_ZF = get_byte_bit(alu->alu_zf, 0);
		break;
	case 0x02:
		inMSWh_ZF = get_byte_bit(alu->alu_zf & get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWh), MSW_ZF), 0);
		break;

	case 0x03:
		inMSWh_ZF = get_byte_bit(z_bus, 0);
		break;
	}

	return inMSWh_ZF;
}

SOL1_BYTE refresh_MSWh_CF(sol1_cpu *sol1_cpu, sol1_alu_8bit *alu, SOL1_BYTE z_bus, SOL1_BYTE cf_in_src) {

	SOL1_BYTE inMSWh_CF = 0x00;

	switch (cf_in_src & 0b00000111) {
	case 0x00:
		inMSWh_CF = get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWh), MSW_CF);
		break;

	case 0x01:
		inMSWh_CF = get_byte_bit(alu->alu_final_cf, 0);
		break;

	case 0x02:
		inMSWh_CF = get_byte_bit(alu->alu_output, 0);
		break;

	case 0x03:
		inMSWh_CF = get_byte_bit(z_bus, 1);
		break;

	case 0x04:
		inMSWh_CF = get_byte_bit(alu->alu_output, 7);
		break;
	}

	return inMSWh_CF;
}

SOL1_BYTE refresh_MSWh_SF(sol1_cpu *sol1_cpu, sol1_alu_8bit *alu, SOL1_BYTE z_bus, SOL1_BYTE sf_in_src) {

	SOL1_BYTE inMSWh_SF = 0x00;

	switch (sf_in_src & 0b00000011) {
	case 0x00:

		inMSWh_SF = get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWh), MSW_SF);
		break;

	case 0x01:
		inMSWh_SF = get_byte_bit(z_bus, 7);
		break;

	case 0x02:
		inMSWh_SF = 0x00;
		break;

	case 0x03:
		inMSWh_SF = get_byte_bit(z_bus, 2);
		break;
	}

	return inMSWh_SF;
}

SOL1_BYTE refresh_MSWh_OF(sol1_cpu *sol1_cpu, sol1_alu_8bit *alu, SOL1_BYTE z_bus, SOL1_BYTE u_sf, SOL1_BYTE of_in_src) {

	SOL1_BYTE inMSWh_OF = 0x00;

	switch (of_in_src & 0b00000111) {
	case 0x00:
		inMSWh_OF = get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWh), MSW_OF);
		break;

	case 0x01:
		inMSWh_OF = get_byte_bit(alu->alu_of, 0);
		break;

	case 0x02:
		inMSWh_OF = get_byte_bit(z_bus, 7);
		break;

	case 0x03:
		inMSWh_OF = get_byte_bit(z_bus, 3);
		break;

	case 0x04:
		inMSWh_OF = get_byte_bit(u_sf, 0) != get_byte_bit(z_bus, 7);
		break;
	}

	return inMSWh_OF;
}


