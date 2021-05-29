#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include <time.h>

#include "sol1_cpu.h"
#include "utils.h"

/*
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
*/

void SOL1_CPU::init()
{
	
	
	
	sol1_alu_init(&this->alu);
	this->rom.init();

	this->microcode.init();

	reset();
	
	this->display_reg_load = 0;
	this->DEBUG_MICROCODE = INI_DEBUG_MICROCODE;
	this->DEBUG_UADDRESSER = INI_DEBUG_UADDRESSER;
	this->DEBUG_UADDER = INI_DEBUG_UADDER;
	this->DEBUG_UFLAGS = INI_DEBUG_UFLAGS;
	this->DEBUG_REGISTERS = INI_DEBUG_REGISTERS;

	this->DEBUG_BUSES = INI_DEBUG_BUSES;
	this->DEBUG_ALU = INI_DEBUG_ALU;
	this->DEBUG_RDMEM = INI_DEBUG_RDMEM;
	this->DEBUG_WRMEM = INI_DEBUG_WRMEM;
	this->DEBUG_MEMORY = INI_DEBUG_MEMORY;
	this->DEBUG_LITE = INI_DEBUG_LITE;
	this->DEBUG_UART = INI_DEBUG_UART;
	this->DEBUG_IDE = INI_DEBUG_IDE;
	this->DEBUG_RTC = INI_DEBUG_RTC;
	this->DEBUG_TIMER = INI_DEBUG_TIMER;

	this->SERVER = INI_SERVER;
	this->DEBUG_LOG_OPCODE = INI_DEBUG_LOG_OPCODE;
}

void SOL1_CPU::reset()
{
	
	//DATA REGISTERS
	SOL1_REGISTERS::reset(this->registers.Al, this->registers.Ah); // AX (16bit) Accumulator	(Ah/Al)
	SOL1_REGISTERS::reset(this->registers.Bl, this->registers.Bh); // BX (16bit) Base		(Bh/Bl)
	SOL1_REGISTERS::reset(this->registers.Cl, this->registers.Ch); // CX (16bit) Counter		(Ch/Cl)
	SOL1_REGISTERS::reset(this->registers.Dl, this->registers.Dh); // DX (16bit) Data		(Dh/Dl)
	SOL1_REGISTERS::reset(this->registers.Gl, this->registers.Gh); // GX (16bit)	Gh/Gl	General Register(For scratch)

	//Pointer Registers
	SOL1_REGISTERS::reset(this->registers.BPl, this->registers.BPh); // BP (16bit) Base Pointer  (Used to manage stack frames)
	SOL1_REGISTERS::reset(this->registers.SPl, this->registers.SPh); // SP (16bit) Stack Pointer
	SOL1_REGISTERS::reset(this->registers.SSPl, this->registers.SSPh); // SSP (16bit) Supervisor Stack Pointer

	//Index Registers
	SOL1_REGISTERS::reset(this->registers.SIl, this->registers.SIh); // SI (16bit) Source index (Source address for string operations)
	SOL1_REGISTERS::reset(this->registers.DIl, this->registers.DIh); // DI (16bit) Destination Index (Destination address for string operations)
	SOL1_REGISTERS::reset(this->registers.PCl, this->registers.PCh); // PC (16bit) Program Counter

	SOL1_REGISTERS::reset(this->registers.TDRl, this->registers.TDRh); // TDR (16bit) Temporary Data Register
	this->registers.PTB.reset();  // PTB (8bit) = Page table base

	SOL1_REGISTERS::reset(this->registers.MSWl, this->registers.MSWh); // 

	SOL1_REGISTERS::reset(this->registers.MARl, this->registers.MARh); // 
	SOL1_REGISTERS::reset(this->registers.MDRl, this->registers.MDRh); // 

	//JumpZ80(z80, z80->registers.PC);

	this->BKPT = 0xFFFF; // Breakpoint
}

void SOL1_CPU::display_registers() {
	
	SOL1_MWORD memADDR = SOL1_REGISTERS::value(this->registers.PCl, this->registers.PCh);
	SOL1_BYTE opcode = get_current_memory()[memADDR];

	printf(" DATA REGISTERS                                        | POINTER REGISTERS\n");
	printf(" *A=%04x", SOL1_REGISTERS::value(this->registers.Al, this->registers.Ah));
	printf(" | ");
	printf("B=%04x", SOL1_REGISTERS::value(this->registers.Bl, this->registers.Bh));
	printf(" | ");
	printf("C=%04x", SOL1_REGISTERS::value(this->registers.Cl, this->registers.Ch));
	printf(" | ");
	printf("D=%04x", SOL1_REGISTERS::value(this->registers.Dl, this->registers.Dh));
	printf(" | ");

	printf("G=%04x", SOL1_REGISTERS::value(this->registers.Gl, this->registers.Gh));
	printf(" | ");
	printf("BP=%04x", SOL1_REGISTERS::value(this->registers.BPl, this->registers.BPh));
	printf(" | ");
	printf("SP=%04x", SOL1_REGISTERS::value(this->registers.SPl, this->registers.SPh));
	printf(" | ");
	printf("SSP=%04x", SOL1_REGISTERS::value(this->registers.SSPl, this->registers.SSPh));
	printf("\n\n");

	printf(" MEMORY REGISTERS       INDEX REGISTERS\n");
	printf(" *MAR=%04x", SOL1_REGISTERS::value(this->registers.MARl, this->registers.MARh));
	printf(" | ");
	printf("MDR=%04x", SOL1_REGISTERS::value(this->registers.MDRl, this->registers.MDRh));
	printf(" | ");
	printf("SI=%04x", SOL1_REGISTERS::value(this->registers.SIl, this->registers.SIh));
	printf(" | ");
	printf("DI=%04x", SOL1_REGISTERS::value(this->registers.DIl, this->registers.DIh));
	printf(" | ");
	printf("PC=%04x", SOL1_REGISTERS::value(this->registers.PCl, this->registers.PCh));
	printf(" | ");


		printf("@PC=%02x:%s", opcode, &this->rom.rom_desc[0x400000 + (opcode * 256)]);

		if (memADDR < get_current_memory_size() - 3)
			printf(" [%02x%02x%02x]", get_current_memory()[memADDR], get_current_memory()[memADDR + 1], get_current_memory()[memADDR + 2]);

		else if (memADDR < get_current_memory_size() - 2)
			printf(" [%02x%02x]", get_current_memory()[memADDR],
				get_current_memory()[memADDR + 1]
			);
		else if (memADDR < get_current_memory_size() - 1)
			printf(" [%02x]", get_current_memory()[memADDR]);
	

	printf("\n\n");

	printf("                              SPECIAL REGISTERS\n");
	printf(" *FLAGS="); print_byte_bin(this->registers.MSWh.value());	printf(":"); mswh_flags_desc();
	printf(" | IR=%02x", this->microcode.IR.value());
	printf(" | TDR=%04x", SOL1_REGISTERS::value(this->registers.TDRl, this->registers.TDRh));
	printf(" | PTB=%02x", this->registers.PTB.value());
	printf("\n");
	printf(" *STATS="); print_byte_bin(this->registers.MSWl.value());
	printf(":       ");
	mswl_status_desc();
	printf("\n\n");
}

void SOL1_CPU::display_registers_lite() {


	printf("*   A=%04x", SOL1_REGISTERS::value(this->registers.Al, this->registers.Ah));
	printf(" | ");
	printf("  B=%04x", SOL1_REGISTERS::value(this->registers.Bl, this->registers.Bh));
	printf(" | ");
	printf("  C=%04x", SOL1_REGISTERS::value(this->registers.Cl, this->registers.Ch));
	printf(" | ");
	printf("  D=%04x", SOL1_REGISTERS::value(this->registers.Dl, this->registers.Dh));
	printf(" | ");
	printf("  G=%04x", SOL1_REGISTERS::value(this->registers.Gl, this->registers.Gh));
	printf("\n");

	printf("*  SI=%04x", SOL1_REGISTERS::value(this->registers.SIl, this->registers.SIh));
	printf(" | ");
	printf(" DI=%04x", SOL1_REGISTERS::value(this->registers.DIl, this->registers.DIh));
	printf(" | ");
	printf(" PC=%04x", SOL1_REGISTERS::value(this->registers.PCl, this->registers.PCh));
	printf(" | ");





		SOL1_MWORD memADDR = SOL1_REGISTERS::value(this->registers.PCl, this->registers.PCh);
		SOL1_BYTE opcode = get_current_memory()[memADDR];

		printf("@PC=%02x:%s", opcode, &this->rom.rom_desc[0x400000 + (opcode * 256)]);

		if (memADDR < get_current_memory_size() - 3)
			printf(" [%02x%02x%02x]", get_current_memory()[memADDR], get_current_memory()[memADDR + 1], get_current_memory()[memADDR + 2]);

		else if (memADDR < get_current_memory_size() - 2)
			printf(" [%02x%02x]", get_current_memory()[memADDR],
				get_current_memory()[memADDR + 1]
			);
		else if (memADDR < get_current_memory_size() - 1)
			printf(" [%02x]", get_current_memory()[memADDR]);
	
	printf("\n");



	printf("* MAR=%04x", SOL1_REGISTERS::value(this->registers.MARl, this->registers.MARh));
	printf(" | ");
	printf("MDR=%04x", SOL1_REGISTERS::value(this->registers.MDRl, this->registers.MDRh));
	printf(" | ");
	printf("TDR=%04x", SOL1_REGISTERS::value(this->registers.TDRl, this->registers.TDRh));
	printf(" | ");
	printf("PTB=%02x", this->registers.PTB.value());
	printf("\n");

	printf("*  BP=%04x", SOL1_REGISTERS::value(this->registers.BPl, this->registers.BPh));
	printf(" | ");
	printf(" SP=%04x", SOL1_REGISTERS::value(this->registers.SPl, this->registers.SPh));
	printf(" | ");
	printf("SSP=%04x", SOL1_REGISTERS::value(this->registers.SSPl, this->registers.SSPh));
	printf("\n");

	printf("* FLAGS="); print_byte_bin(this->registers.MSWh.value());
	printf("   | ");
	printf("STATUS="); print_byte_bin(this->registers.MSWl.value());
	printf("\n");
	printf("* IR: %02x\n", this->microcode.IR.value());
	printf("Flags: "); mswh_flags_desc(); printf("\n");
	printf("Status: "); mswl_status_desc(); printf("\n");
}




void  SOL1_CPU::memory_display()
{

	int i = 0, j = 0;
	SOL1_DWORD PC = SOL1_REGISTERS::value(this->registers.PCl, this->registers.PCh);

	if (this->memory.debug_manual_offset == 0x00) {
		if (0 + this->memory.debug_mem_offset > PC || PC >= 256 + this->memory.debug_mem_offset)
			this->memory.debug_mem_offset = (PC / 0x10) * 0x10;
	}

	printf("\n        ");

	for (i = 0; i < 16; i++)
		printf("%02x ", i);

	printf("\n\n %04x ", this->memory.debug_mem_offset);

	for (i = 0 + this->memory.debug_mem_offset; i < 256 + this->memory.debug_mem_offset; i++) {
		if (i % 16 == 0)
			if (PC == i)
				printf(" *");
			else
				printf("  ");
		if (PC == i || PC - 1 == i)
			printf("%02x*", this->memory.memory[i]);
		else
			printf("%02x ", this->memory.memory[i]);

		if ((i + 1) % 16 == 0 && i <= 255 + this->memory.debug_mem_offset) {
			printf("  |");
			for (j = (i + 1) - 16; j < (i + 1); j++) {
				if (this->memory.memory[j] < 0x20)
					printf(".");
				else
					printf("%c", this->memory.memory[j]);
			}
			printf("|");

			if (i < 255 + this->memory.debug_mem_offset)
				printf("\n %04x ", i + 1);
			else
				printf("\n");

		}
	}
}


void  SOL1_CPU::mswh_flags_desc() {

	SOL1_BYTE b = this->registers.MSWh.value();
	printf(" [");
	if (get_byte_bit(b, MSW_ZF) != 0x00)
		printf("Z"); else printf(" ");
	if (get_byte_bit(b, MSW_CF) != 0x00)
		printf("C"); else printf(" ");
	if (get_byte_bit(b, MSW_SF) != 0x00)
		printf("S"); else printf(" ");
	if (get_byte_bit(b, MSW_OF) != 0x00)
		printf("O"); else printf(" ");
	printf("]");
}




void SOL1_CPU::mswl_status_desc() {

	SOL1_BYTE b = this->registers.MSWl.value();
	if (get_byte_bit(b, MSW_DMA_ACK) != 0x00)
		printf(" | dma_ack ");
	if (get_byte_bit(b, MSW_INTERRUPT_ENABLE) != 0x00)
		printf(" | interrupt_enable ");
	if (get_byte_bit(b, MSW_CPU_MODE) != 0x00)
		printf(" | cpu_mode ");
	if (get_byte_bit(b, MSW_PAGING_EN) != 0x00)
		printf(" | paging_en ");
	if (get_byte_bit(b, MSW_HALT) != 0x00)
		printf(" | halt ");
	if (get_byte_bit(b, MSW_DISPLAY_REG_LOAD) != 0x00)
		printf(" | display_reg_load ");
	if (get_byte_bit(b, MSW_DIR) != 0x00)
		printf(" | dir ");
}




void SOL1_CPU::load_microcode_from_rom() {

	this->microcode.mccycle.next = this->rom.roms[0][this->microcode.u_ad_bus] & 0b00000011;///////////////////////

	this->microcode.u_offset = ((this->rom.roms[1][this->microcode.u_ad_bus] & 0b00000001) << 6) |/////////
		((this->rom.roms[0][this->microcode.u_ad_bus] >> 2) & 0b00111111);////////////////////

	this->microcode.mccycle.cond_inv = (this->rom.roms[1][this->microcode.u_ad_bus] >> 1) & 0b00000001;
	this->microcode.mccycle.cond_flags_src = (this->rom.roms[1][this->microcode.u_ad_bus] >> 2) & 0b00000001;
	this->microcode.mccycle.cond_sel = (this->rom.roms[1][this->microcode.u_ad_bus] >> 3) & 0b00001111;

	this->microcode.u_escape = this->rom.roms[1][this->microcode.u_ad_bus] >> 7 & 0b00000001; //ESCAPE
	this->microcode.u_esc_in_src = 0;

	this->microcode.uzf_in_src = this->rom.roms[2][this->microcode.u_ad_bus] & 0b00000011;/////////////////
	this->microcode.ucf_in_src = (this->rom.roms[2][this->microcode.u_ad_bus] >> 2) & 0b00000011;//////////
	this->microcode.usf_in_src = (this->rom.roms[2][this->microcode.u_ad_bus] >> 4) & 0b00000001;//////////
	this->microcode.uof_in_src = (this->rom.roms[2][this->microcode.u_ad_bus] >> 5) & 0b00000001;//////////
	this->microcode.mccycle.ir_wrt = (this->rom.roms[2][this->microcode.u_ad_bus] >> 6) & 0b00000001;/////////////////
	this->microcode.mccycle.status_wrt = (this->rom.roms[2][this->microcode.u_ad_bus] >> 7) & 0b00000001;/////////////

	this->microcode.mccycle.shift_src = this->rom.roms[3][this->microcode.u_ad_bus] & 0b00000111;/////////////////////
	this->microcode.mccycle.zbus_out_src = (this->rom.roms[3][this->microcode.u_ad_bus] >> 3) & 0b00000011;///////////

	this->alu.alu_a_src = ((this->rom.roms[3][this->microcode.u_ad_bus] >> 5) & 0b00000111) | ///////
		((this->rom.roms[4][this->microcode.u_ad_bus] & 0b00000111) << 3);/////////////////////

	this->alu.alu_op = (this->rom.roms[4][this->microcode.u_ad_bus] >> 3) & 0b00001111;//////////////
	this->alu.alu_mode = (this->rom.roms[4][this->microcode.u_ad_bus] >> 7) & 0b00000001;////////////

	this->alu.alu_cf_in_src = (this->rom.roms[5][this->microcode.u_ad_bus]) & 0b00000011;////////////
	this->alu.alu_cf_in_inv = (this->rom.roms[5][this->microcode.u_ad_bus] >> 2) & 0b00000001;///////
	this->microcode.mccycle.zf_in_src = (this->rom.roms[5][this->microcode.u_ad_bus] >> 3) & 0b00000011;///////////////


	this->alu.alu_cf_out_inv = (this->rom.roms[5][this->microcode.u_ad_bus] >> 5) & 0b00000001;//////
	this->microcode.mccycle.cf_in_src = ((this->rom.roms[5][this->microcode.u_ad_bus] >> 6) & 0b00000011) |////////////
		((this->rom.roms[6][this->microcode.u_ad_bus] & 0b00000001) << 2);/////////////////////

	this->microcode.mccycle.sf_in_src = (this->rom.roms[6][this->microcode.u_ad_bus] >> 1) & 0b00000011;///////////////

	this->microcode.mccycle.of_in_src = (this->rom.roms[6][this->microcode.u_ad_bus] >> 3) & 0b00000111;///////////////

	this->microcode.mccycle.rd = (this->rom.roms[6][this->microcode.u_ad_bus] >> 6) & 0b00000001;//////////////////////
	this->microcode.mccycle.wr = (this->rom.roms[6][this->microcode.u_ad_bus] >> 7) & 0b00000001;//////////////////////

	this->alu.alu_b_src = this->rom.roms[7][this->microcode.u_ad_bus] & 0b00000111;//////////////////

	this->microcode.mccycle.display_reg_load = (this->rom.roms[7][this->microcode.u_ad_bus] >> 3) & 0b00000001;////
	this->microcode.mccycle.dl_wrt = (this->rom.roms[7][this->microcode.u_ad_bus] >> 4) & 0b00000001;//////////////
	this->microcode.mccycle.dh_wrt = (this->rom.roms[7][this->microcode.u_ad_bus] >> 5) & 0b00000001;//////////////
	this->microcode.mccycle.cl_wrt = (this->rom.roms[7][this->microcode.u_ad_bus] >> 6) & 0b00000001;//////////////
	this->microcode.mccycle.ch_wrt = (this->rom.roms[7][this->microcode.u_ad_bus] >> 7) & 0b00000001;//////////////
	this->microcode.mccycle.bl_wrt = (this->rom.roms[8][this->microcode.u_ad_bus] >> 0) & 0b00000001;//////////////
	this->microcode.mccycle.bh_wrt = (this->rom.roms[8][this->microcode.u_ad_bus] >> 1) & 0b00000001;//////////////
	this->microcode.mccycle.al_wrt = (this->rom.roms[8][this->microcode.u_ad_bus] >> 2) & 0b00000001;//////////////
	this->microcode.mccycle.ah_wrt = (this->rom.roms[8][this->microcode.u_ad_bus] >> 3) & 0b00000001;//////////////
	this->microcode.mccycle.mdr_in_src = (this->rom.roms[8][this->microcode.u_ad_bus] >> 4) & 0b00000001;//////////
	this->microcode.mccycle.mdr_out_src = (this->rom.roms[8][this->microcode.u_ad_bus] >> 5) & 0b00000001;/////////
	this->microcode.mccycle.mdr_out_en = (this->rom.roms[8][this->microcode.u_ad_bus] >> 6) & 0b00000001;//////////
	this->microcode.mccycle.mdrl_wrt = (this->rom.roms[8][this->microcode.u_ad_bus] >> 7) & 0b00000001;////////////
	this->microcode.mccycle.mdrh_wrt = (this->rom.roms[9][this->microcode.u_ad_bus] >> 0) & 0b00000001;////////////
	this->microcode.mccycle.tdrl_wrt = (this->rom.roms[9][this->microcode.u_ad_bus] >> 1) & 0b00000001;////////////
	this->microcode.mccycle.tdrh_wrt = (this->rom.roms[9][this->microcode.u_ad_bus] >> 2) & 0b00000001;////////////
	this->microcode.mccycle.dil_wrt = (this->rom.roms[9][this->microcode.u_ad_bus] >> 3) & 0b00000001;/////////////
	this->microcode.mccycle.dih_wrt = (this->rom.roms[9][this->microcode.u_ad_bus] >> 4) & 0b00000001;/////////////
	this->microcode.mccycle.sil_wrt = (this->rom.roms[9][this->microcode.u_ad_bus] >> 5) & 0b00000001;/////////////
	this->microcode.mccycle.sih_wrt = (this->rom.roms[9][this->microcode.u_ad_bus] >> 6) & 0b00000001;/////////////
	this->microcode.mccycle.marl_wrt = (this->rom.roms[9][this->microcode.u_ad_bus] >> 7) & 0b00000001;////////////
	this->microcode.mccycle.marh_wrt = (this->rom.roms[10][this->microcode.u_ad_bus] >> 0) & 0b00000001;///////////
	this->microcode.mccycle.bpl_wrt = (this->rom.roms[10][this->microcode.u_ad_bus] >> 1) & 0b00000001;////////////
	this->microcode.mccycle.bph_wrt = (this->rom.roms[10][this->microcode.u_ad_bus] >> 2) & 0b00000001;////////////
	this->microcode.mccycle.pcl_wrt = (this->rom.roms[10][this->microcode.u_ad_bus] >> 3) & 0b00000001;////////////
	this->microcode.mccycle.pch_wrt = (this->rom.roms[10][this->microcode.u_ad_bus] >> 4) & 0b00000001;////////////
	this->microcode.mccycle.spl_wrt = (this->rom.roms[10][this->microcode.u_ad_bus] >> 5) & 0b00000001;////////////
	this->microcode.mccycle.sph_wrt = (this->rom.roms[10][this->microcode.u_ad_bus] >> 6) & 0b00000001;////////////

	this->microcode.mccycle.int_vector_wrt = (this->rom.roms[11][this->microcode.u_ad_bus] >> 1) & 0b00000001;/////
	this->microcode.mccycle.mask_flags_wrt = (this->rom.roms[11][this->microcode.u_ad_bus] >> 2) & 0b00000001;/////
	this->microcode.mccycle.mar_in_src = (this->rom.roms[11][this->microcode.u_ad_bus] >> 3) & 0b00000001;/////////
	this->microcode.mccycle.int_ack = (this->rom.roms[11][this->microcode.u_ad_bus] >> 4) & 0b00000001;
	this->microcode.mccycle.clear_all_ints = (this->rom.roms[11][this->microcode.u_ad_bus] >> 5) & 0b00000001;
	this->microcode.mccycle.ptb_wrt = (this->rom.roms[11][this->microcode.u_ad_bus] >> 6) & 0b00000001;////////////
	this->microcode.mccycle.pagtbl_ram_we = (this->rom.roms[11][this->microcode.u_ad_bus] >> 7) & 0b00000001;//////
	this->microcode.mccycle.mdr_to_pagtbl_en = (this->rom.roms[12][this->microcode.u_ad_bus] >> 0) & 0b00000001;///
	this->microcode.mccycle.force_user_ptb = (this->rom.roms[12][this->microcode.u_ad_bus] >> 1) & 0b00000001;

	this->microcode.mccycle.gl_wrt = (this->rom.roms[12][this->microcode.u_ad_bus] >> 6) & 0b00000001;/////////////
	this->microcode.mccycle.gh_wrt = (this->rom.roms[12][this->microcode.u_ad_bus] >> 7) & 0b00000001;/////////////

	this->microcode.mccycle.imm = this->rom.roms[13][this->microcode.u_ad_bus]; ///////////////////////////////////
}




SOL1_BYTE SOL1_CPU::refresh_MSWh_ZF(SOL1_BYTE z_bus, SOL1_BYTE zf_in_src) {

	SOL1_BYTE inMSWh_ZF = 0x00;

	switch (zf_in_src & 0b00000011) {
	case 0x00:
		inMSWh_ZF = get_byte_bit(this->registers.MSWh.value(), MSW_ZF);
		break;

	case 0x01:
		inMSWh_ZF = get_byte_bit(this->alu.alu_zf, 0);
		break;
	case 0x02:
		inMSWh_ZF = get_byte_bit(this->alu.alu_zf & get_byte_bit(this->registers.MSWh.value(), MSW_ZF), 0);
		break;

	case 0x03:
		inMSWh_ZF = get_byte_bit(z_bus, 0);
		break;
	}

	return inMSWh_ZF;
}


SOL1_BYTE  SOL1_CPU::refresh_MSWh_CF( SOL1_BYTE z_bus, SOL1_BYTE cf_in_src) {

	SOL1_BYTE inMSWh_CF = 0x00;

	switch (cf_in_src & 0b00000111) {
	case 0x00:
		inMSWh_CF = get_byte_bit(this->registers.MSWh.value(), MSW_CF);
		break;

	case 0x01:
		inMSWh_CF = get_byte_bit(this->alu.alu_final_cf, 0);
		break;

	case 0x02:
		inMSWh_CF = get_byte_bit(this->alu.alu_output, 0);
		break;

	case 0x03:
		inMSWh_CF = get_byte_bit(z_bus, 1);
		break;

	case 0x04:
		inMSWh_CF = get_byte_bit(this->alu.alu_output, 7);
		break;
	}

	return inMSWh_CF;
}

SOL1_BYTE SOL1_CPU::refresh_MSWh_SF(SOL1_BYTE z_bus, SOL1_BYTE sf_in_src) {

	SOL1_BYTE inMSWh_SF = 0x00;

	switch (sf_in_src & 0b00000011) {
	case 0x00:

		inMSWh_SF = get_byte_bit(this->registers.MSWh.value(), MSW_SF);
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

SOL1_BYTE SOL1_CPU::refresh_MSWh_OF(SOL1_BYTE z_bus, SOL1_BYTE u_sf, SOL1_BYTE of_in_src) {

	SOL1_BYTE inMSWh_OF = 0x00;

	switch (of_in_src & 0b00000111) {
	case 0x00:
		inMSWh_OF = get_byte_bit(this->registers.MSWh.value(), MSW_OF);
		break;

	case 0x01:
		inMSWh_OF = get_byte_bit(this->alu.alu_of, 0);
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



SOL1_BYTE *SOL1_CPU::get_current_memory() {
	SOL1_BYTE *memory;
	if (!check_byte_bit(this->registers.MSWl.value(), MSW_PAGING_EN))
		memory = this->memory.memory;

	else
		memory = this->memory.main_memory;

	return memory;
}

int SOL1_CPU::get_current_memory_size() {
	
	if (!check_byte_bit(this->registers.MSWl.value(), MSW_PAGING_EN))
		return SOL1_BIOS_MEMORY_SIZE;

	else
		return SOL1_MAIN_MEMORY_SIZE;
}



void SOL1_CPU::mc_seq_update() {



	//CLOCK LOW
	this->microcode.addresser(this->registers, this->DEBUG_UADDRESSER);


	/*
	int i = 0;
	int j = 0;
	for (i = 0; i < 15; i++) {
		//microcode = microcode | (sol1_cpu->rom.roms[i][u_ad_bus] << (8 * i));

		if (sol1_cpu->DEBUG_MICROCODE && sol1_cpu->rom.roms[i][this->u_ad_bus] != 0x00)
			for (j = 0; j < 8; j++)
				if ((sol1_cpu->rom.roms[i][this->u_ad_bus] >> j) & 0x1 != 0)
					printf("%s, ", SOL1_ROM_CONTROL_LIST[(i * 8) + j]);
	}
	if (sol1_cpu->DEBUG_MICROCODE) printf("\n");
	SOL1_BYTE opcode = u_ad_bus;
	int cycle = 0;
	int p = opcode * SOL1_ROM_CYCLES_PER_INSTR + cycle; <<<< u_ad_bus
	*/

	////////////////////////////////////////////////////////////////////////////

	load_microcode_from_rom();

	if (this->DEBUG_MICROCODE) {
		printf("***** MICROCODE\n");
		//printf("U-ADDRESS: ");  print_word_bin(this->u_ad_bus); printf("\n");		
		//printf("OPCODE: %02x (cycle %02x)\n", (this->u_ad_bus / 64), (this->u_ad_bus % 64));
		//printf("microcode: \n");
		this->rom.display_current_cycles_desc((this->microcode.u_ad_bus / 64), (this->microcode.u_ad_bus % 64));
		printf("\n");
	}

}