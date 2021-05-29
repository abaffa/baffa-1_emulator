#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include <chrono>
#ifdef __MINGW32__
#include <time.h>
#include <pthread.h> 
#endif



// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")


#include "console_local.h"
#include "config.h"
#include "debugmenu_main.h"

#include "sol1_alu.h"
#include "sol1_cpu.h"
#include "sol1_memory.h"
#include "sol1_rom.h"
#include "sol1_microcode.h"
#include "hw_uart.h"
#include "hw_rtc.h"
#include "hw_ide.h"
#include "hw_tty.h"

#include "utils.h"

#include "sol1_register_8bit.h"

#include "ttl74series.h"

#include <math.h>


#include "hashtable.h"
#include "tasm_opcode.h"

SOL1_BYTE apagar = 0;
int gambi_ide = 0;
int gambi_ide_total = 0;
int gambi_ide_read = 0;
int gambi_last_op = 0;

Queue* uart_out;
SOCKET server;

struct hw_rtc hw_rtc;
struct hw_uart hw_uart;
struct hw_ide hw_ide;



HashTable* ht_opcodes;




/// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//boot sequence: bios, boot, kernel, shell
/// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

using namespace std::chrono;

int step = 0;
int microcodestep = 0;
SOL1_MWORD oldPC = -1;
SOL1_BYTE oldOP = -1;


struct bus {

	SOL1_BYTE data_bus;

	SOL1_BYTE k_bus; // input pra alu x e y
	SOL1_BYTE w_bus; // input pra alu x e y
	SOL1_BYTE x_bus; //alu entrada
	SOL1_BYTE y_bus; //alu entrada
	SOL1_BYTE z_bus; //alu saida
};


//////////////////////////
//////////////////////////
//////////////////////////
//////////////////////////
//////////////////////////




void refresh_int(sol1_cpu *sol1_cpu) {


	SOL1_BYTE int_req_0 = 0x00;
	SOL1_BYTE int_req_1 = 0x00;
	SOL1_BYTE int_req_2 = 0x00;
	SOL1_BYTE int_req_3 = 0x00;
	SOL1_BYTE int_req_4 = 0x00;
	SOL1_BYTE int_req_5 = 0x00;
	SOL1_BYTE int_req_6 = 0x00;
	SOL1_BYTE int_req_7 = 0x00;

	SOL1_BYTE not_clear_all_ints = get_byte_bit(~sol1_cpu->microcode.mccycle.clear_all_ints, 0x00);

	SOL1_BYTE int_vector_0 = 0x01;
	SOL1_BYTE int_vector_1 = 0x01;
	SOL1_BYTE int_vector_2 = 0x01;
	SOL1_BYTE int_vector_3 = 0x01;
	SOL1_BYTE int_vector_4 = 0x01;
	SOL1_BYTE int_vector_5 = 0x01;
	SOL1_BYTE int_vector_6 = 0x01;
	SOL1_BYTE int_vector_7 = 0x01;

	SOL1_BYTE int_status_0 = 0x00;
	SOL1_BYTE int_status_1 = 0x00;
	SOL1_BYTE int_status_2 = 0x00;
	SOL1_BYTE int_status_3 = 0x00;
	SOL1_BYTE int_status_4 = 0x00;
	SOL1_BYTE int_status_5 = 0x00;
	SOL1_BYTE int_status_6 = 0x00;
	SOL1_BYTE int_status_7 = 0x00;


	if (sol1_cpu->microcode.mccycle.int_ack == 0x01) {
		int_vector_0 = get_byte_bit(~get_byte_bit(sol1_cpu->microcode.mccycle.int_vector, 0), 0);
		int_vector_1 = get_byte_bit(~get_byte_bit(sol1_cpu->microcode.mccycle.int_vector, 1), 0);
		int_vector_2 = get_byte_bit(~get_byte_bit(sol1_cpu->microcode.mccycle.int_vector, 2), 0);
		int_vector_3 = get_byte_bit(~get_byte_bit(sol1_cpu->microcode.mccycle.int_vector, 3), 0);
		int_vector_4 = get_byte_bit(~get_byte_bit(sol1_cpu->microcode.mccycle.int_vector, 4), 0);
		int_vector_5 = get_byte_bit(~get_byte_bit(sol1_cpu->microcode.mccycle.int_vector, 5), 0);
		int_vector_6 = get_byte_bit(~get_byte_bit(sol1_cpu->microcode.mccycle.int_vector, 6), 0);
		int_vector_7 = get_byte_bit(~get_byte_bit(sol1_cpu->microcode.mccycle.int_vector, 7), 0);
	}

	SOL1_BYTE int_clr_0 = (int_vector_0 & not_clear_all_ints);
	SOL1_BYTE int_clr_1 = (int_vector_1 & not_clear_all_ints);
	SOL1_BYTE int_clr_2 = (int_vector_2 & not_clear_all_ints);
	SOL1_BYTE int_clr_3 = (int_vector_3 & not_clear_all_ints);
	SOL1_BYTE int_clr_4 = (int_vector_4 & not_clear_all_ints);
	SOL1_BYTE int_clr_5 = (int_vector_5 & not_clear_all_ints);
	SOL1_BYTE int_clr_6 = (int_vector_6 & not_clear_all_ints);
	SOL1_BYTE int_clr_7 = (int_vector_7 & not_clear_all_ints);


	if (int_clr_0 == 0x01 && get_byte_bit(sol1_cpu->microcode.mccycle.int_req, 0) == 0x01)
		int_status_0 = get_byte_bit(~get_byte_bit(sol1_cpu->microcode.mccycle.int_status, 0), 0);

	if (int_clr_1 == 0x01 && get_byte_bit(sol1_cpu->microcode.mccycle.int_req, 1) == 0x01)
		int_status_1 = get_byte_bit(~get_byte_bit(sol1_cpu->microcode.mccycle.int_status, 1), 0);

	if (int_clr_2 == 0x01 && get_byte_bit(sol1_cpu->microcode.mccycle.int_req, 2) == 0x01)
		int_status_2 = get_byte_bit(~get_byte_bit(sol1_cpu->microcode.mccycle.int_status, 2), 0);

	if (int_clr_3 == 0x01 && get_byte_bit(sol1_cpu->microcode.mccycle.int_req, 3) == 0x01)
		int_status_3 = get_byte_bit(~get_byte_bit(sol1_cpu->microcode.mccycle.int_status, 3), 0);

	if (int_clr_4 == 0x01 && get_byte_bit(sol1_cpu->microcode.mccycle.int_req, 4) == 0x01)
		int_status_4 = get_byte_bit(~get_byte_bit(sol1_cpu->microcode.mccycle.int_status, 4), 0);

	if (int_clr_5 == 0x01 && get_byte_bit(sol1_cpu->microcode.mccycle.int_req, 5) == 0x01)
		int_status_5 = get_byte_bit(~get_byte_bit(sol1_cpu->microcode.mccycle.int_status, 5), 0);

	if (int_clr_6 == 0x01 && get_byte_bit(sol1_cpu->microcode.mccycle.int_req, 6) == 0x01)
		int_status_6 = get_byte_bit(~get_byte_bit(sol1_cpu->microcode.mccycle.int_status, 6), 0);

	if (int_clr_7 == 0x01 && get_byte_bit(sol1_cpu->microcode.mccycle.int_req, 7) == 0x01)
		int_status_7 = get_byte_bit(~get_byte_bit(sol1_cpu->microcode.mccycle.int_status, 7), 0);



	sol1_cpu->microcode.mccycle.int_status = (int_status_7 << 7) | (int_status_6 << 6) | (int_status_5 << 5) | (int_status_4 << 4) | (int_status_3 << 3) | (int_status_2 << 2) | (int_status_1 << 1) | (int_status_0 << 0);


	if (sol1_cpu->microcode.mccycle.int_request == 0x00 && sol1_cpu->microcode.mccycle.int_vector_wrt == 0x00) {

		SOL1_BYTE iii = ~(get_byte_bit(~(int_status_7 & get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.INT_MASKS), 7)), 0)) |
			~(get_byte_bit(~(int_status_6 & get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.INT_MASKS), 6)), 0) << 1) |
			~(get_byte_bit(~(int_status_5 & get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.INT_MASKS), 5)), 0) << 2) |
			~(get_byte_bit(~(int_status_4 & get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.INT_MASKS), 4)), 0) << 3) |
			~(get_byte_bit(~(int_status_3 & get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.INT_MASKS), 3)), 0) << 4) |
			~(get_byte_bit(~(int_status_2 & get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.INT_MASKS), 2)), 0) << 5) |
			~(get_byte_bit(~(int_status_1 & get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.INT_MASKS), 1)), 0) << 6) |
			~(get_byte_bit(~(int_status_0 & get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.INT_MASKS), 0)), 0) << 6);

		sol1_cpu->microcode.mccycle.int_vector = iii;
	}

}



SOL1_BYTE bus_tristate(sol1_cpu *sol1_cpu) {

	//if (get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWl), MSW_HALT)) // GAMBIARRA ENQUANTO SEM INTERRUPCOES
	//	sol1_register_8bit_set(sol1_cpu->registers.MSWl, sol1_register_8bit_value(sol1_cpu->registers.MSWl) & 0b11101110);

	return (get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWl), MSW_DMA_ACK) |
		get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWl), MSW_HALT)) & 0b00000001; //IC151
}

SOL1_BYTE bus_rd(sol1_cpu *sol1_cpu) {

	if (bus_tristate(sol1_cpu) != 0x00)
		return 0x01;

	return (~sol1_cpu->microcode.mccycle.rd) & 0b00000001;
}

SOL1_BYTE bus_wr(sol1_cpu *sol1_cpu) {
	if (bus_tristate(sol1_cpu) != 0x00)
		return 0x01;

	return (~sol1_cpu->microcode.mccycle.wr) & 0b00000001;
}

SOL1_BYTE buffer_rd(sol1_cpu *sol1_cpu) {
	return bus_rd(sol1_cpu);
}

SOL1_BYTE buffer_wr(sol1_cpu *sol1_cpu) {
	return bus_wr(sol1_cpu);
}

SOL1_BYTE buffer_mem_io(sol1_cpu *sol1_cpu) {
	return sol1_cpu->microcode.mccycle.memory_io;
}


unsigned long read_address_bus(sol1_cpu *sol1_cpu) {
	unsigned long address_bus = 0x00;

	if (bus_tristate(sol1_cpu) == 0x00)

		if (get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWl), MSW_PAGING_EN) == 0x00) {
			address_bus = (sol1_register_8bit_value(sol1_cpu->memory.MARh) << 8) | sol1_register_8bit_value(sol1_cpu->memory.MARl);

			sol1_cpu->microcode.mccycle.memory_io = 0;
			sol1_cpu->microcode.mccycle.page_present = 0;
			sol1_cpu->microcode.mccycle.page_writable = 0;
		}
		else {
			//sol1_cpu->DEBUG_WRMEM = 1;
			//sol1_cpu->DEBUG_RDMEM = 1;

			SOL1_MWORD ptb_mem_addr = ((sol1_register_8bit_value(sol1_cpu->memory.MARh) >> 3) & 0b00011111);

			if (page_table_addr_src(sol1_cpu) == 0x01)
				ptb_mem_addr = ptb_mem_addr | sol1_register_8bit_value(sol1_cpu->registers.PTB) << 5;

			if (sol1_cpu->microcode.mccycle.mdr_to_pagtbl_en == 0x0)
				address_bus = ((sol1_cpu->memory.memchip1[ptb_mem_addr] & 0b00000011) << 8) | sol1_cpu->memory.memchip0[ptb_mem_addr];
			else
				address_bus = (sol1_register_8bit_value(sol1_cpu->memory.MDRh) << 8) | sol1_register_8bit_value(sol1_cpu->memory.MDRl);

			address_bus = (address_bus << 11) | ((sol1_register_8bit_value(sol1_cpu->memory.MARh) & 0b00000111) << 8) | sol1_register_8bit_value(sol1_cpu->memory.MARl);

			sol1_cpu->microcode.mccycle.memory_io = get_byte_bit(sol1_cpu->memory.memchip1[ptb_mem_addr], 3);
			sol1_cpu->microcode.mccycle.page_present = get_byte_bit(sol1_cpu->memory.memchip1[ptb_mem_addr], 3);
			sol1_cpu->microcode.mccycle.page_writable = get_byte_bit(sol1_cpu->memory.memchip1[ptb_mem_addr], 4);
		}


	/////MEMORIA AQUI
	// ~oe = output enabled
	// ~we = write enabled
	// ~ce = chip enabled

	return address_bus;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

SOL1_BYTE refresh_k_bus(SOL1_BYTE alu_b_src, sol1_cpu *sol1_cpu) {

	//IC92 //I103 //IC116

	SOL1_BYTE k_bus = 0x00;
	switch (alu_b_src & 0b00000011) {
	case 0x00: k_bus = sol1_register_8bit_value(sol1_cpu->memory.MDRl); break;
	case 0x01: k_bus = sol1_register_8bit_value(sol1_cpu->memory.MDRh); break;
	case 0x02: k_bus = sol1_register_8bit_value(sol1_cpu->registers.TDRl); break;
	case 0x03: k_bus = sol1_register_8bit_value(sol1_cpu->registers.TDRh); break;
	}

	return k_bus;
}

SOL1_BYTE refresh_w_bus(
	SOL1_BYTE panel_regsel,
	SOL1_BYTE alu_a_src,
	SOL1_BYTE display_reg_load,
	SOL1_BYTE int_vector,
	SOL1_BYTE int_masks,
	SOL1_BYTE int_status,
	sol1_cpu *sol1_cpu) {

	//IC125 //IC118 //IC3   //IC2  //IC9  //IC42  //IC6   //IC20 //IC5  //IC80  //IC41  //IC44
	//IC30  //IC130 //IC56  //IC62 //IC53 //IC133 //IC68  //IC69 //IC67 //IC141 //IC81
	//IC82  //IC71  //IC144 //IC85 //IC86 //IC84  //IC152 //IC88 //IC89 //IC86  //IC160

	SOL1_BYTE w_bus = 0x00;

	SOL1_BYTE inABC = 0x00;
	SOL1_BYTE inAB = 0x00;

	if (bus_tristate(sol1_cpu) == 0x00 & display_reg_load == 0x00) {
		inABC = alu_a_src & 0b00000111;
		inAB = get_byte_bit(alu_a_src, 3) | set_byte_bit(get_byte_bit(alu_a_src, 4), 1);
	}
	else {
		inABC = panel_regsel & 0b00000111;
		inAB = get_byte_bit(panel_regsel, 3) | set_byte_bit(get_byte_bit(panel_regsel, 4), 1);
	}


	if (inAB == 0x00) {
		switch (inABC) {
		case 0x00:
			w_bus = sol1_register_8bit_value(sol1_cpu->registers.Al);
			break;
		case 0x01:
			w_bus = sol1_register_8bit_value(sol1_cpu->registers.Ah);
			break;
		case 0x02:
			w_bus = sol1_register_8bit_value(sol1_cpu->registers.Bl);
			break;
		case 0x03:
			w_bus = sol1_register_8bit_value(sol1_cpu->registers.Bh);
			break;
		case 0x04:
			w_bus = sol1_register_8bit_value(sol1_cpu->registers.Cl);
			break;
		case 0x05:
			w_bus = sol1_register_8bit_value(sol1_cpu->registers.Ch);
			break;
		case 0x06:
			w_bus = sol1_register_8bit_value(sol1_cpu->registers.Dl);
			/*
			if (get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWl), MSW_PAGING_EN) == 0)
				w_bus = sol1_register_8bit_value(sol1_cpu->registers.Dl);
			else
				w_bus = sol1_register_8bit_value(sol1_cpu->registers.DIl);

			*/
			break;
		case 0x07:
			w_bus = sol1_register_8bit_value(sol1_cpu->registers.Dh);
			/*
			if (get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWl), MSW_PAGING_EN) == 0)
				w_bus = sol1_register_8bit_value(sol1_cpu->registers.Dh);
			else
				w_bus = sol1_register_8bit_value(sol1_cpu->registers.DIh);
				*/
			break;
		}

	}
	else if (inAB == 0x01) {
		switch (inABC) {
		case 0x00:
			w_bus = sol1_register_8bit_value(sol1_cpu->registers.SPl);
			break;
		case 0x01:
			w_bus = sol1_register_8bit_value(sol1_cpu->registers.SPh);
			break;
		case 0x02:
			w_bus = sol1_register_8bit_value(sol1_cpu->registers.BPl);
			break;
		case 0x03:
			w_bus = sol1_register_8bit_value(sol1_cpu->registers.BPh);
			break;
		case 0x04:
			w_bus = sol1_register_8bit_value(sol1_cpu->registers.SIl);
			break;
		case 0x05:
			w_bus = sol1_register_8bit_value(sol1_cpu->registers.SIh);
			break;
		case 0x06:
			w_bus = sol1_register_8bit_value(sol1_cpu->registers.DIl);
			break;
		case 0x07:
			w_bus = sol1_register_8bit_value(sol1_cpu->registers.DIh);
			break;
		}
	}
	else if (inAB == 0x02) {
		switch (inABC) {
		case 0x00:
			w_bus = sol1_register_8bit_value(sol1_cpu->registers.PCl);
			break;
		case 0x01:
			w_bus = sol1_register_8bit_value(sol1_cpu->registers.PCh);
			break;
		case 0x02:
			w_bus = sol1_register_8bit_value(sol1_cpu->memory.MARl);
			break;
		case 0x03:
			w_bus = sol1_register_8bit_value(sol1_cpu->memory.MARh);
			break;
		case 0x04:
			w_bus = sol1_register_8bit_value(sol1_cpu->memory.MDRl);
			break;
		case 0x05:
			w_bus = sol1_register_8bit_value(sol1_cpu->memory.MDRh);
			break;
		case 0x06:
			w_bus = sol1_register_8bit_value(sol1_cpu->registers.TDRl);
			break;
		case 0x07:
			w_bus = sol1_register_8bit_value(sol1_cpu->registers.TDRh);
			break;
		}
	}
	else if (inAB == 0x03) {
		switch (inABC) {
		case 0x00:
			w_bus = sol1_register_8bit_value(sol1_cpu->registers.SSPl);
			break;
		case 0x01:
			w_bus = sol1_register_8bit_value(sol1_cpu->registers.SSPh);
			break;
		case 0x02:
			w_bus = int_vector;
			break;
		case 0x03:
			w_bus = int_masks;
			break;
		case 0x04:
			w_bus = int_status;
			break;
		}
	}
	return w_bus;
}


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////







SOL1_BYTE get_current_opcode(sol1_cpu *sol1_cpu) {
	return (sol1_cpu->microcode.u_ad_bus / 64);
}

SOL1_BYTE get_current_opcode_cycle(sol1_cpu *sol1_cpu) {
	return  (sol1_cpu->microcode.u_ad_bus % 64);
}


SOL1_BYTE last_opcode = 0xFF;


FILE *fa;
void save_to_log(char *str)
{

	//FILE *fa = fopen("File1.txt", "a");
	if (fa == NULL)
	{
		printf("can not open target file\n");
		exit(1);
	}

	fprintf(fa, "%s", str);
	fflush(fa);
	//fputc(str, fa);

//fclose(fa);

}

void disassembly_current_opcode(sol1_cpu *sol1_cpu) {
	SOL1_BYTE current_opcode = get_current_opcode(sol1_cpu);
	if (last_opcode != current_opcode) {
		char temp[4];
		char line[255];

		sprintf(temp, "%02x", current_opcode);
		Tasm_Opcode *tt = (Tasm_Opcode*)ht_search(ht_opcodes, temp);
		if (tt != NULL) {

			SOL1_MWORD memADDR = sol1_registers_value(sol1_cpu->registers.PCl, sol1_cpu->registers.PCh);

			sprintf(line, "    %04x]\t%02x:%s", memADDR - (strlen(temp) / 2), current_opcode, tt->desc);

			if (tt->size > 1) {

				int i = 0;
				sprintf(line + strlen(line), " (");

				for (i = tt->size - 2; i >= 0; i--)
					sprintf(line + strlen(line), "%02x", get_current_memory(sol1_cpu)[memADDR + i]);

				sprintf(line + strlen(line), ")");
			}

			sprintf(line + strlen(line), "\n");

			save_to_log(line);
		}
		/*
		else if (current_opcode == 0 && sol1_registers_value(sol1_cpu->registers.PCl, sol1_cpu->registers.PCh) == 0)
		{
			save_to_log("########################\n");
			save_to_log("# STARTING - RESET CPU #\n");
			save_to_log("########################\n");

			printf("RESTARTING ...pressione uma tecla para continuar...\n");
			getch();
		}
		*/
		last_opcode = current_opcode;
	}
}

void mc_sequencer(
	sol1_cpu *sol1_cpu,
	sol1_alu_8bit *alu,
	struct bus *bus,
	long *runtime_counter
) {



	//printf("#################################################################################\n");
	//printf("# Runtime Counter: %d\n", (*runtime_counter)++);
	//printf("#################################################################################\n");


	////////////////////////////////////////////////////////////////////////////
	mc_seq_update(sol1_cpu, alu);
	refresh_int(sol1_cpu);


	if (sol1_cpu->DEBUG_LOG_OPCODE & get_current_opcode(sol1_cpu) > 0)
		//if ((get_current_opcode(sol1_cpu) != 0 || (get_current_opcode(sol1_cpu) == 0 && (get_current_opcode_cycle(sol1_cpu) < 10 | get_current_opcode_cycle(sol1_cpu) > 14))))
		disassembly_current_opcode(sol1_cpu);


	if (!(sol1_cpu->rom.bkpt_opcode == 0 && sol1_cpu->rom.bkpt_cycle == 0) &&
		((sol1_cpu->microcode.u_ad_bus / 64) == sol1_cpu->rom.bkpt_opcode &&
		(sol1_cpu->microcode.u_ad_bus % 64) == sol1_cpu->rom.bkpt_cycle))
	{
		printf(" Microcode Breakpoint | Starting Opcode/Cycle:%02x%02x.\n", sol1_cpu->rom.bkpt_opcode, sol1_cpu->rom.bkpt_cycle);
		sol1_cpu->DEBUG_MICROCODE = 1;
		sol1_cpu->DEBUG_REGISTERS = 1;
		sol1_cpu->DEBUG_ALU = 1;
		step = 1;
		if (sol1_cpu->DEBUG_MICROCODE) {
			printf("***** MICROCODE\n");
			//printf("U-ADDRESS: ");  print_word_bin(sol1_cpu->microcode.u_ad_bus); printf("\n");		
			//printf("OPCODE: %02x (cycle %02x)\n", (sol1_cpu->microcode.u_ad_bus / 64), (sol1_cpu->microcode.u_ad_bus % 64));
			//printf("microcode: \n");
			printf("* U_FLAGS="); print_byte_bin(sol1_register_8bit_value(sol1_cpu->microcode.U_FLAGS)); printf("\n");
			sol1_rom_display_current_cycles_desc(&sol1_cpu->rom, (sol1_cpu->microcode.u_ad_bus / 64), (sol1_cpu->microcode.u_ad_bus % 64));
			printf("\n");
		}
		debugmenu_main(sol1_cpu);
	}


	////////////////////////////////////////////////////////////////////////////
	// MEMORY READ

	//IC7  //IC24 //IC19 //183

	if (sol1_cpu->microcode.mccycle.mdr_out_en & 0b00000001 == 0x01) {
		switch (sol1_cpu->microcode.mccycle.mdr_out_src & 0b00000001) {
		case 0x00:
			bus->data_bus = sol1_register_8bit_value(sol1_cpu->memory.MDRl);
			break;
		case 0x01:
			bus->data_bus = sol1_register_8bit_value(sol1_cpu->memory.MDRh);
			break;
		}
	}
	//244 MICROCODE SIGNALS NEED
	//INVERTER BECAUSE THEY GET RESET TO
	//ZERO AT BEGINNING, AND 244 ARE LOW ACTIVE.
	//THIS CAUSES PROBLEMS ON RESET.

	////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////


	if (buffer_rd(sol1_cpu) == 0x00) {
		unsigned long mem_addr = read_address_bus(sol1_cpu);


		if (mem_addr < 0xFF80 || mem_addr > 0xFFFF) {

			bus->data_bus = get_current_memory(sol1_cpu)[mem_addr];

			if (sol1_cpu->DEBUG_RDMEM && get_current_opcode(sol1_cpu) > 0) {
				char log[255];
				if (bus->data_bus == 0x00)
					sprintf(log, "]]] READ RAM [%04x] = %02x \'\\0\'\n", mem_addr, bus->data_bus);
				else if (bus->data_bus == 0x09)
					sprintf(log, "]]] READ RAM [%04x] = %02x \'\\t\'\n", mem_addr, bus->data_bus);
				else if (bus->data_bus == 0x0a)
					sprintf(log, "]]] READ RAM [%04x] = %02x \'\\r\'\n", mem_addr, bus->data_bus);
				else if (bus->data_bus == 0x0d)
					sprintf(log, "]]] READ RAM [%04x] = %02x \'\\n\'\n", mem_addr, bus->data_bus);
				else
					sprintf(log, "]]] READ RAM [%04x] = %02x \'%c\'\n", mem_addr, bus->data_bus, bus->data_bus);
				save_to_log(log);
			}
		}
		else if (mem_addr >= 0xFF80 && mem_addr <= 0xFFFF) {

			//RTC I/O bank = FFA0 to FFAF
			if (mem_addr >= 0xFFA0 && mem_addr <= 0xFFAF) {
				bus->data_bus = hw_rtc.data[mem_addr - 0xFFA0];
			}

			//UART SERVICES INTERRUPT
			else if (mem_addr >= 0xFF80 && mem_addr <= 0xFF85) {

				hw_uart_get_lsr(&hw_uart);

				if (mem_addr - 0xFF80 == 0) {
					hw_uart_read(&hw_uart);
				}

				bus->data_bus = hw_uart.data[mem_addr - 0xFF80];

				if (sol1_cpu->DEBUG_UART & get_current_opcode(sol1_cpu) > 0) {
					char log_uart[255];
					hw_uart_print(&hw_uart, "READ", (mem_addr - 0xFF80), log_uart);
					save_to_log(log_uart);
				}

			}


			if (mem_addr >= 0xFFD0 && mem_addr <= 0xFFD7) {


				if (hw_ide.data[7] == 0b00001000 && mem_addr - 0xFFD0 == 0) {

					gambi_ide_total = hw_ide.data[2];

					unsigned long sec_address_lba = hw_ide.data[3];
					sec_address_lba = sec_address_lba | ((unsigned long)hw_ide.data[4]) << 8;
					sec_address_lba = sec_address_lba | ((unsigned long)hw_ide.data[5]) << 16;
					sec_address_lba = sec_address_lba | ((unsigned long)(hw_ide.data[6] & 0b00001111)) << 24;

					unsigned long sec_address_byte = sec_address_lba * 512;

					if (sec_address_byte < SOL1_IDE_MEMORY_SIZE) {
						hw_ide.data[0] = hw_ide.memory[sec_address_byte + gambi_ide_read];

						gambi_ide_read++;


						if (gambi_ide_read > gambi_ide_total * 512)
						{
							gambi_ide_total = 0;
							gambi_ide_read = 0;
							gambi_last_op = 0;
							hw_ide.data[7] = 0x00;
						}
					}
					else {
						gambi_ide_total = 0;
						gambi_ide_read = 0;
						gambi_last_op = 0;
						hw_ide.data[7] = 0x24;
					}
				}

				//hw_ide_read(sol1_cpu, uart_out);
				bus->data_bus = hw_ide.data[mem_addr - 0xFFD0];


				if (sol1_cpu->DEBUG_IDE & get_current_opcode(sol1_cpu) > 0) {
					char log_ide[255];
					hw_ide_print(&hw_ide, "READ ", (mem_addr - 0xFFD0), log_ide);
					save_to_log(log_ide);
				}

			}

		}
	}

	////////////////////////////////////////////////////////////////////////////
	// W = X
	bus->w_bus = refresh_w_bus(sol1_cpu->microcode.mccycle.panel_regsel,
		alu->alu_a_src, sol1_cpu->microcode.mccycle.display_reg_load && sol1_cpu->display_reg_load,
		sol1_cpu->microcode.mccycle.int_vector, sol1_register_8bit_value(sol1_cpu->registers.INT_MASKS), sol1_cpu->microcode.mccycle.int_status,
		sol1_cpu);

	////////////////////////////////////////////////////////////////////////////
	// K = Y
	bus->k_bus = refresh_k_bus(alu->alu_b_src, sol1_cpu);
	////////////////////////////////////////////////////////////////////////////
	//IC184 //IC204 //IC170 //IC179 //IC181 //IC182

	if (get_byte_bit(alu->alu_a_src, 5) == 0x00)
		bus->x_bus = bus->w_bus;

	else {

		switch ((alu->alu_a_src & 0b00000011)) {
		case 0x00:

			bus->x_bus = set_byte_bit(get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWh), MSW_ZF), 0) |
				set_byte_bit(get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWh), MSW_CF), 1) |
				set_byte_bit(get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWh), MSW_SF), 2) |
				set_byte_bit(get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWh), MSW_OF), 3) |
				set_byte_bit(get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWh), MSW_12), 4) |
				set_byte_bit(get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWh), MSW_13), 5) |
				set_byte_bit(get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWh), MSW_14), 6) |
				set_byte_bit(get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWh), MSW_15), 7);
			break;

		case 0x01:

			bus->x_bus = get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWl), MSW_DMA_ACK) |
				set_byte_bit(get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWl), MSW_INTERRUPT_ENABLE), 1) |
				set_byte_bit(get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWl), MSW_CPU_MODE), 2) |
				set_byte_bit(get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWl), MSW_PAGING_EN), 3) |
				set_byte_bit(get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWl), MSW_HALT), 4) |
				set_byte_bit(get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWl), MSW_DISPLAY_REG_LOAD), 5) |
				set_byte_bit(get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWl), MSW_14), 6) |
				set_byte_bit(get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWl), MSW_DIR), 7);
			break;

		case 0x02:
			bus->x_bus = sol1_register_8bit_value(sol1_cpu->registers.Gl);
			break;

		case 0x03:
			bus->x_bus = sol1_register_8bit_value(sol1_cpu->registers.Gh);
			break;
		}
	}

	////////////////////////////////////////////////////////////////////////////
	//IC98 //IC22
	bus->y_bus = (alu->alu_b_src == 0x00) ? sol1_cpu->microcode.mccycle.imm : bus->k_bus;


	///////////////////////////////////////////////////////////////////////////
	//SOL1_BYTE clk = 0x1;
	//SOL1_BYTE rst = 0x1;
	//clk //IC28
	if (sol1_cpu->microcode.mccycle.ir_wrt == 0x00) // SETA IR ANTES DA OPERACAO
		sol1_register_8bit_set(sol1_cpu->microcode.IR, bus->data_bus);
	////////////////////////////////////////////////////////////////////////////
	// ALU

	//sol1_u_flags(sol1_cpu, alu, bus->z_bus);
	//update_final_condition(sol1_cpu);

	refresh_MSWh_ZF(sol1_cpu, alu, bus->z_bus, sol1_cpu->microcode.mccycle.zf_in_src);
	refresh_MSWh_CF(sol1_cpu, alu, bus->z_bus, sol1_cpu->microcode.mccycle.cf_in_src);
	refresh_MSWh_SF(sol1_cpu, alu, bus->z_bus, sol1_cpu->microcode.mccycle.sf_in_src);
	refresh_MSWh_OF(sol1_cpu, alu, bus->z_bus, sol1_cpu->microcode.u_sf, sol1_cpu->microcode.mccycle.of_in_src);

	sol1_u_flags(sol1_cpu, alu, bus->z_bus);
	update_final_condition(sol1_cpu);



	SOL1_BYTE alu_result = ALU_EXEC(alu, bus->x_bus, bus->y_bus,
		alu->alu_op,
		alu->alu_mode,
		alu->alu_cf_in_src,
		alu->alu_cf_in_inv,
		alu->alu_cf_out_inv,
		sol1_cpu->microcode.u_cf,
		get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWh), MSW_CF),
		sol1_cpu->microcode.mccycle.shift_src,
		sol1_cpu->microcode.mccycle.zbus_out_src, sol1_cpu->DEBUG_ALU);

	////////////////////////////////////////////////////////////////////////////


	bus->z_bus = alu_result;


	//#######################

	if (buffer_wr(sol1_cpu) == 0x00) {
		unsigned long mem_addr = read_address_bus(sol1_cpu);





		if (mem_addr < 0xFF80 || mem_addr > 0xFFFF) {

			if (sol1_cpu->DEBUG_WRMEM && get_current_opcode(sol1_cpu) > 0)
			{
				char log[255];
				if (bus->data_bus == 0x00)
					sprintf(log, "]]] WRITE RAM [%04x] = %02x \'\\0'\n", mem_addr, bus->data_bus);
				else if (bus->data_bus == 0x09)
					sprintf(log, "]]] WRITE RAM [%04x] = %02x \'\\t'\n", mem_addr, bus->data_bus);
				else if (bus->data_bus == 0x0a)
					sprintf(log, "]]] WRITE RAM [%04x] = %02x \'\\r\'\n", mem_addr, bus->data_bus);
				else if (bus->data_bus == 0x0d)
					sprintf(log, "]]] WRITE RAM [%04x] = %02x \'\\n\'\n", mem_addr, bus->data_bus);
				else
					sprintf(log, "]]] WRITE RAM [%04x] = %02x \'%c\'\n", mem_addr, bus->data_bus, bus->data_bus);
				save_to_log(log);
			}

			get_current_memory(sol1_cpu)[mem_addr] = bus->data_bus;

		}
		else if (mem_addr >= 0xFF80 && mem_addr <= 0xFFFF) {

			//RTC I/O bank = FFA0 to FFAF
			if (mem_addr >= 0xFFA0 && mem_addr <= 0xFFAF) {
				hw_rtc.data[mem_addr - 0xFFA0] = bus->data_bus;


				if (sol1_cpu->DEBUG_RTC & get_current_opcode(sol1_cpu) > 0) {
					char log_rtc[255];
					hw_rtc_print(&hw_rtc, log_rtc);
				}
			}

			//UART SERVICES INTERRUPT
			else if (mem_addr >= 0xFF80 && mem_addr <= 0xFF85) {

				hw_uart_get_lsr(&hw_uart);

				if (mem_addr - 0xFF80 == 0 && hw_uart.data[5] == 0x20) {
					hw_uart_send(&hw_uart, bus->data_bus);

					int i;
					for (i = 0; i < 10; i++) {
						if (clients[i].running == 1) {
							struct net_data *net_data = (struct net_data*)malloc(sizeof(struct net_data));
							net_data->data = bus->data_bus;
							queue_insert(clients[i].uart_out, net_data);

						}
					}

				}
				else
					hw_uart.data[mem_addr - 0xFF80] = bus->data_bus;

				if (sol1_cpu->DEBUG_UART & get_current_opcode(sol1_cpu) > 0) {
					//hw_uart_write(sol1_cpu, uart_out, bus->data_bus);
					char log_uart[255];
					hw_uart_print(&hw_uart, "WRITE", (mem_addr - 0xFF80), log_uart);
					save_to_log(log_uart);
				}

			}

			if (mem_addr >= 0xFFD0 && mem_addr <= 0xFFD7) {

				hw_ide.data[mem_addr - 0xFFD0] = bus->data_bus;

				if (sol1_cpu->DEBUG_IDE & get_current_opcode(sol1_cpu) > 0) {
					char log_ide[255];
					hw_ide_print(&hw_ide, "WRITE", (mem_addr - 0xFFD0), log_ide);
					save_to_log(log_ide);
				}




				if (hw_ide.data[7] == 0b00001000 && mem_addr - 0xFFD0 == 0) {

					gambi_ide_total = hw_ide.data[2];

					unsigned long sec_address_lba = hw_ide.data[3];
					sec_address_lba = sec_address_lba | ((unsigned long)hw_ide.data[4]) << 8;
					sec_address_lba = sec_address_lba | ((unsigned long)hw_ide.data[5]) << 16;
					sec_address_lba = sec_address_lba | ((unsigned long)(hw_ide.data[6] & 0b00001111)) << 24;

					unsigned long sec_address_byte = sec_address_lba * 512;

					if (sec_address_byte < SOL1_IDE_MEMORY_SIZE) {
						hw_ide.memory[sec_address_byte + gambi_ide_read] = hw_ide.data[0];

						gambi_ide_read++;


						if (gambi_ide_read > gambi_ide_total * 512)
						{
							gambi_ide_total = 0;
							gambi_ide_read = 0;
							gambi_last_op = 0;
							hw_ide.data[7] = 0x00;

							save_disk(hw_ide.memory);
						}
					}
					else {
						gambi_ide_total = 0;
						gambi_ide_read = 0;
						gambi_last_op = 0;
						hw_ide.data[7] = 0x34;
					}
				}


				// SET HD NEW STATUS AFTER LOG
				if (hw_ide.data[7] == 0x04) { // RESET IDE
					hw_ide.data[7] = 0x0; // 0x80 ==busy// is ready again
				}

				else if (hw_ide.data[7] == 0xEF) { // SET FEATURE COMMAND
					hw_ide.data[7] = 0x00; // is ready again
				}

				else if (hw_ide.data[7] == 0xE6) { // SLEEP
					hw_ide.data[7] = 0x00;// zerar 
					//hw_ide.data[7] = 0x80; // is ready again
				}

				else if (hw_ide.data[7] == 0x20) { // read sector cmd
					hw_ide.data[7] = 0b00001000;
					gambi_ide_total = 0;
					gambi_ide_read = 0;
					gambi_last_op = 0;
				}
				else if (hw_ide.data[7] == 0x30) { // write sector cmd
					hw_ide.data[7] = 0b00001000;
					gambi_ide_total = 0;
					gambi_ide_read = 0;
					gambi_last_op = 0;
				}
			}
		}

	}


	//}
	//#######################
	//IC86B //IC58B //IC86C //IC241 //IC14 //IC255 //IC23

	//
	SOL1_BYTE inMSWh_ZF = refresh_MSWh_ZF(sol1_cpu, alu, bus->z_bus, sol1_cpu->microcode.mccycle.zf_in_src);
	SOL1_BYTE inMSWh_CF = refresh_MSWh_CF(sol1_cpu, alu, bus->z_bus, sol1_cpu->microcode.mccycle.cf_in_src);
	SOL1_BYTE inMSWh_SF = refresh_MSWh_SF(sol1_cpu, alu, bus->z_bus, sol1_cpu->microcode.mccycle.sf_in_src);
	SOL1_BYTE inMSWh_OF = refresh_MSWh_OF(sol1_cpu, alu, bus->z_bus, sol1_cpu->microcode.u_sf, sol1_cpu->microcode.mccycle.of_in_src);

	//

	if (0x01) { // ~RST
		//IC206
		SOL1_BYTE inMSW_H = set_byte_bit(inMSWh_ZF, 0) | set_byte_bit(inMSWh_CF, 1) | set_byte_bit(inMSWh_SF, 2) | set_byte_bit(inMSWh_OF, 3);
		sol1_register_8bit_set(sol1_cpu->registers.MSWh, inMSW_H);
	}

	if (sol1_cpu->microcode.mccycle.status_wrt == 0x00) {
		SOL1_BYTE oldStatus = sol1_register_8bit_value(sol1_cpu->registers.MSWl);
		sol1_register_8bit_set(sol1_cpu->registers.MSWl, bus->z_bus);
	}



	///////////////////////////////////////////////////////////////////////////
	// READ DATA
	//DATA REGISTERS
	if (sol1_cpu->microcode.mccycle.ah_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.Ah, bus->z_bus);
	if (sol1_cpu->microcode.mccycle.al_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.Al, bus->z_bus);

	if (sol1_cpu->microcode.mccycle.bh_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.Bh, bus->z_bus);
	if (sol1_cpu->microcode.mccycle.bl_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.Bl, bus->z_bus);

	if (sol1_cpu->microcode.mccycle.ch_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.Ch, bus->z_bus);
	if (sol1_cpu->microcode.mccycle.cl_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.Cl, bus->z_bus);

	if (sol1_cpu->microcode.mccycle.dh_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.Dh, bus->z_bus);
	if (sol1_cpu->microcode.mccycle.dl_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.Dl, bus->z_bus);

	/*
	if (get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWl), MSW_PAGING_EN) == 0) {
		if (sol1_cpu->microcode.mccycle.dh_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.Dh, bus->z_bus);
		if (sol1_cpu->microcode.mccycle.dl_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.Dl, bus->z_bus);
	}
	else {
		if (sol1_cpu->microcode.mccycle.dh_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.DIh, bus->z_bus);
		if (sol1_cpu->microcode.mccycle.dl_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.DIl, bus->z_bus);
	}
	*/
	if (sol1_cpu->microcode.mccycle.gh_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.Gh, bus->z_bus);
	if (sol1_cpu->microcode.mccycle.gl_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.Gl, bus->z_bus);

	//Pointer Registers
	if (sol1_cpu->microcode.mccycle.bph_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.BPh, bus->z_bus);
	if (sol1_cpu->microcode.mccycle.bpl_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.BPl, bus->z_bus);


	if (get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWl), MSW_CPU_MODE) == 0) {

		if (sol1_cpu->microcode.mccycle.sph_wrt == 0x00 | sol1_cpu->microcode.mccycle.ssph_wrt == 0x00) {
			sol1_register_8bit_set(sol1_cpu->registers.SPh, bus->z_bus);
			sol1_register_8bit_set(sol1_cpu->registers.SSPh, bus->z_bus);
		}
		if (sol1_cpu->microcode.mccycle.spl_wrt == 0x00 | sol1_cpu->microcode.mccycle.sspl_wrt == 0x00) {
			sol1_register_8bit_set(sol1_cpu->registers.SPl, bus->z_bus);
			sol1_register_8bit_set(sol1_cpu->registers.SSPl, bus->z_bus);
		}
	}
	else {
		if (sol1_cpu->microcode.mccycle.sph_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.SPh, bus->z_bus);
		if (sol1_cpu->microcode.mccycle.spl_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.SPl, bus->z_bus);


		if (sol1_cpu->microcode.mccycle.ssph_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.SSPh, bus->z_bus);
		if (sol1_cpu->microcode.mccycle.sspl_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.SSPl, bus->z_bus);
	}

	//Index Registers
	if (sol1_cpu->microcode.mccycle.sih_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.SIh, bus->z_bus);
	if (sol1_cpu->microcode.mccycle.sil_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.SIl, bus->z_bus);

	if (sol1_cpu->microcode.mccycle.dih_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.DIh, bus->z_bus);
	if (sol1_cpu->microcode.mccycle.dil_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.DIl, bus->z_bus);

	if (sol1_cpu->microcode.mccycle.pch_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.PCh, bus->z_bus);
	if (sol1_cpu->microcode.mccycle.pcl_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.PCl, bus->z_bus);

	if (sol1_cpu->microcode.mccycle.tdrh_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.TDRh, bus->z_bus);
	if (sol1_cpu->microcode.mccycle.tdrl_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.TDRl, bus->z_bus);

	if (sol1_cpu->microcode.mccycle.ptb_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.PTB, bus->z_bus);

	/////////////////////////////////////////////////////////////////////////////
	if (sol1_cpu->microcode.mccycle.mask_flags_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.INT_MASKS, bus->z_bus);
	/////////////////////////////////////////////////////////////////////////////
	// SET MDR
	 //IC7 //IC24 //IC19 //IC183

	if (sol1_cpu->microcode.mccycle.mdrl_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->memory.MDRl, sol1_cpu->microcode.mccycle.mdr_in_src == 0x00 ? bus->z_bus : bus->data_bus);
	if (sol1_cpu->microcode.mccycle.mdrh_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->memory.MDRh, sol1_cpu->microcode.mccycle.mdr_in_src == 0x00 ? bus->z_bus : bus->data_bus);
	////////////////////////////////////////////////////////////////////////////
	//MEMORY SET MAR

	if (sol1_cpu->microcode.mccycle.marl_wrt == 0x00) {
		//IC131 //IC128			
		if (sol1_cpu->microcode.mccycle.mar_in_src == 0x00)
			sol1_register_8bit_set(sol1_cpu->memory.MARl, bus->z_bus);
		else
			sol1_register_8bit_set(sol1_cpu->memory.MARl, sol1_register_8bit_value(sol1_cpu->registers.PCl));
	}

	if (sol1_cpu->microcode.mccycle.marh_wrt == 0x00) {
		//IC129 //IC132			
		if (sol1_cpu->microcode.mccycle.mar_in_src == 0x00)
			sol1_register_8bit_set(sol1_cpu->memory.MARh, bus->z_bus);
		else
			sol1_register_8bit_set(sol1_cpu->memory.MARh, sol1_register_8bit_value(sol1_cpu->registers.PCh));
	}

	if (sol1_cpu->microcode.mccycle.pagtbl_ram_we == 0x01)
	{
		SOL1_MWORD ptb_mem_addr = ((sol1_register_8bit_value(sol1_cpu->memory.MARh) >> 3) & 0b00011111);
		if (page_table_addr_src(sol1_cpu) == 0x01)
			ptb_mem_addr = ptb_mem_addr | sol1_register_8bit_value(sol1_cpu->registers.PTB) << 5;

		if (sol1_cpu->microcode.mccycle.mdr_to_pagtbl_en == 0x1) {
			sol1_cpu->memory.memchip0[ptb_mem_addr] = sol1_register_8bit_value(sol1_cpu->memory.MDRl);
			sol1_cpu->memory.memchip1[ptb_mem_addr] = sol1_register_8bit_value(sol1_cpu->memory.MDRh);

		}

		else {
			sol1_cpu->memory.memchip0[ptb_mem_addr] = 0;
			sol1_cpu->memory.memchip1[ptb_mem_addr] = 0;
		}
	}




	////////////////////////////////////////////////////////////////////////////

	if (sol1_cpu->DEBUG_BUSES) {
		printf("***** BUS\n");
		printf("* u_ad_bus    : "); print_word_bin(sol1_cpu->microcode.u_ad_bus); printf("\n");
		printf("* address bus : "); print_byte_bin(read_address_bus(sol1_cpu) >> 16); printf(" ");  print_word_bin(read_address_bus(sol1_cpu)); printf("\n");
		printf("*  data_bus   |");
		printf("    k_bus    |");
		printf("    w_bus    |");
		printf("    x_bus    |");
		printf("    y_bus    |");
		printf("    z_bus    ");
		printf("\n");
		printf("  "); print_byte_bin(bus->data_bus);
		printf(" | "); print_byte_bin(bus->k_bus);
		printf(" | "); print_byte_bin(bus->w_bus);
		printf(" | "); print_byte_bin(bus->x_bus);
		printf(" | "); print_byte_bin(bus->y_bus);
		printf(" | "); print_byte_bin(bus->z_bus);
		printf("\n");
		printf("\n");
	}

	if (sol1_cpu->DEBUG_MEMORY) {
		//printf("***** MEMORY\n");
		sol1_cpu_memory_display(sol1_cpu);
	}

	if (sol1_cpu->DEBUG_REGISTERS) {
		printf("***** REGISTERS\n");
		sol1_cpu_display_registers(sol1_cpu);

	}


	//CLOCK HIGH





	refresh_MSWh_ZF(sol1_cpu, alu, bus->z_bus, sol1_cpu->microcode.mccycle.zf_in_src);
	refresh_MSWh_CF(sol1_cpu, alu, bus->z_bus, sol1_cpu->microcode.mccycle.cf_in_src);
	refresh_MSWh_SF(sol1_cpu, alu, bus->z_bus, sol1_cpu->microcode.mccycle.sf_in_src);
	refresh_MSWh_OF(sol1_cpu, alu, bus->z_bus, sol1_cpu->microcode.u_sf, sol1_cpu->microcode.mccycle.of_in_src);

	sol1_u_flags(sol1_cpu, alu, bus->z_bus);
	update_final_condition(sol1_cpu);

	sol1_u_adder(&sol1_cpu->microcode, sol1_cpu->microcode.mccycle.next, sol1_cpu->microcode.mccycle.final_condition);











	if (sol1_cpu->DEBUG_UADDER) {
		printf("***** U_ADDER\n");
		sol1_microcode_display_u_adder(&sol1_cpu->microcode, sol1_cpu->microcode.mccycle.next, sol1_cpu->microcode.mccycle.final_condition);
		printf("\n");
	}

	if (!(sol1_cpu->rom.bkpt_opcode == 0 && sol1_cpu->rom.bkpt_cycle == 0) &&
		((sol1_cpu->microcode.u_ad_bus / 64) == sol1_cpu->rom.bkpt_opcode &&
		(sol1_cpu->microcode.u_ad_bus % 64) == sol1_cpu->rom.bkpt_cycle))
	{
		printf(" Microcode Breakpoint | Opcode/Cycle:%02x%02x Finished.\n", sol1_cpu->rom.bkpt_opcode, sol1_cpu->rom.bkpt_cycle);
		debugmenu_main(sol1_cpu);
	}

	if ((sol1_cpu->BKPT != 0xFFFF) && (sol1_registers_value(sol1_cpu->memory.MARl, sol1_cpu->memory.MARh) == sol1_cpu->BKPT
		|| sol1_registers_value(sol1_cpu->registers.PCl, sol1_cpu->registers.PCh) == sol1_cpu->BKPT))
	{
		sol1_cpu->DEBUG_MICROCODE = 1;
		sol1_cpu->DEBUG_REGISTERS = 1;
		sol1_cpu->DEBUG_ALU = 1;
		step = 1;
		if (sol1_cpu->DEBUG_MICROCODE) {
			printf("\n***** MICROCODE\n");
			//printf("U-ADDRESS: ");  print_word_bin(sol1_cpu->microcode.u_ad_bus); printf("\n");		
			//printf("OPCODE: %02x (cycle %02x)\n", (sol1_cpu->microcode.u_ad_bus / 64), (sol1_cpu->microcode.u_ad_bus % 64));
			//printf("microcode: \n");
			printf("* U_FLAGS="); print_byte_bin(sol1_register_8bit_value(sol1_cpu->microcode.U_FLAGS)); printf("\n");
			sol1_rom_display_current_cycles_desc(&sol1_cpu->rom, (sol1_cpu->microcode.u_ad_bus / 64), (sol1_cpu->microcode.u_ad_bus % 64));
			printf("\n");
		}

		printf(" Memory Breakpoint | Starting Address:%04x.\n", sol1_cpu->BKPT);
		debugmenu_main(sol1_cpu);
	}
	/*
	else if ((sol1_cpu->BKPT != 0xFFFF) && (sol1_registers_value(sol1_cpu->memory.MARl, sol1_cpu->memory.MARh) == sol1_cpu->BKPT
		|| sol1_registers_value(sol1_cpu->registers.PCl, sol1_cpu->registers.PCh) == sol1_cpu->BKPT))
	{
		printf(" Memory Breakpoint | Address:%04x Finished.\n", sol1_cpu->BKPT);
		debugmenu_main(sol1_cpu);
	}
	*/



	bus->data_bus = 0;
	bus->k_bus = 0;
	bus->w_bus = 0;
	bus->x_bus = 0;
	bus->y_bus = 0;
	//bus->z_bus = 0;




}


void RunCPU(
	sol1_cpu *sol1_cpu,
	sol1_alu_8bit *alu,
	struct bus *bus,
	long *runtime_counter
) {

	while (1) {



		mc_sequencer(sol1_cpu, alu, bus, runtime_counter);

		if (!sol1_cpu->SERVER) {



			if (kbhit()) {
				char dddd = getch();
				if (dddd == 0x1b)
					return;


				if (dddd == '0') {// || 1 ) {

					sol1_cpu->DEBUG_MICROCODE = 1;
					sol1_cpu->DEBUG_UADDRESSER = INI_DEBUG_UADDRESSER;
					sol1_cpu->DEBUG_UADDER = INI_DEBUG_UADDER;
					sol1_cpu->DEBUG_UFLAGS = INI_DEBUG_UFLAGS;
					sol1_cpu->DEBUG_REGISTERS = 1;

					sol1_cpu->DEBUG_BUSES = INI_DEBUG_BUSES;
					sol1_cpu->DEBUG_ALU = 1;
					sol1_cpu->DEBUG_RDMEM = INI_DEBUG_RDMEM;
					sol1_cpu->DEBUG_WRMEM = INI_DEBUG_WRMEM;
					sol1_cpu->DEBUG_MEMORY = INI_DEBUG_MEMORY;
					sol1_cpu->DEBUG_UART = 1;
					sol1_cpu->DEBUG_LITE = 0;
					step = 1;
				}


				hw_uart_receive(&hw_uart, dddd);
				sol1_cpu->microcode.mccycle.int_request = 0x01;
			}
		}

		if (hw_uart.data[5] == 0xFF || hw_uart.data[5] == 0x20)
			if (!queue_empty(hw_uart.uart_in))
				hw_uart_write(&hw_uart);



		// DEBUG UART
		if (sol1_cpu->DEBUG_UART) {
			if (get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWl), MSW_PAGING_EN) == 0x00) {


			}
			else {

				printf("***** UART\n");
				printf("DATA/DLAB0: %02x", sol1_cpu->memory.main_memory[0xFF80]);
				printf(" | ");
				printf("DLAB1: %02x", sol1_cpu->memory.main_memory[0xFF81]);
				printf(" | ");
				printf("Int. En.: %02x", sol1_cpu->memory.main_memory[0xFF82]);
				printf(" | ");
				printf("FIFO Ctl.: %02x", sol1_cpu->memory.main_memory[0xFF83]);
				printf(" | ");
				printf("LINE Ctl.: %02x", sol1_cpu->memory.main_memory[0xFF84]);
				printf(" | ");
				printf("LINE Status: %02x", sol1_cpu->memory.main_memory[0xFF85]);
				printf("\n\n");
			}
		}




		if (sol1_cpu->DEBUG_LITE && ((sol1_cpu->microcode.u_ad_bus / 64) != oldOP || sol1_registers_value(sol1_cpu->registers.PCl, sol1_cpu->registers.PCh) != oldPC)) {

			SOL1_BYTE cur_opcode = (sol1_cpu->microcode.u_ad_bus / 64);
			SOL1_BYTE cur_cycle = (sol1_cpu->microcode.u_ad_bus % 64);
			SOL1_BYTE pc_opcode = get_current_memory(sol1_cpu)[sol1_registers_value(sol1_cpu->registers.PCl, sol1_cpu->registers.PCh)];

			if (!(cur_opcode == 0x00 && cur_cycle == 0x10)) {
				char str[100];
				sprintf(str, "# Opcode=%02x:%s", cur_opcode, &sol1_cpu->rom.rom_desc[0x400000 + (cur_opcode * 256)]);
				printf("%s", rightpad(str, 40));

				printf(" | ");
				printf("Cycle: % 02x", cur_cycle);
				printf(" | ");

				printf("PC: % 04x", sol1_registers_value(sol1_cpu->registers.PCl, sol1_cpu->registers.PCh));
				printf(" | ");
				/*
				printf("@PC=%02x:%s\n", pc_opcode, &sol1_cpu->rom.rom_desc[0x400000 + (pc_opcode * 256)]);
				*/
				printf("MEM:%02x%02x",
					get_current_memory(sol1_cpu)[sol1_registers_value(sol1_cpu->memory.MARl, sol1_cpu->memory.MARh) + 1],
					get_current_memory(sol1_cpu)[sol1_registers_value(sol1_cpu->memory.MARl, sol1_cpu->memory.MARh)]
				);

				printf("\n");

			}
			oldOP = cur_opcode;
			oldPC = sol1_registers_value(sol1_cpu->registers.PCl, sol1_cpu->registers.PCh);
		}

		if (MUX(sol1_cpu) == 0x02 && step == 1) {

			if (!sol1_cpu->DEBUG_LITE) {
				printf("###########################################\n");
				printf("## End OpStep on Opcode/Cycle:%02x%02x. #######\n", (sol1_cpu->microcode.u_ad_bus / 64), (sol1_cpu->microcode.u_ad_bus % 64));
				printf("###########################################\n");
			}
			return;
		}
		else if (step == 1 && !sol1_cpu->DEBUG_LITE) {
			printf("###########################################\n");
		}
		else if (microcodestep == 1) {
			if (!sol1_cpu->DEBUG_LITE) {
				printf("###########################################\n");
				printf("## End MicroStep on Opcode/Cycle:%02x%02x. ####\n", (sol1_cpu->microcode.u_ad_bus / 64), (sol1_cpu->microcode.u_ad_bus % 64));
				printf("###########################################\n");
			}
			return;
		}


	}
	sol1_cpu->microcode.mccycle.next = 0x00;
}


char* loadfile(char *filename, long *size) {

	printf("The filename to load is: %s", filename);

	FILE* f = fopen(filename, "rb");
	if (!f)
	{
		printf(" | Failed to open the file.\n");
		return NULL;
	}

	fseek(f, 0, SEEK_END);
	*size = ftell(f);
	fseek(f, 0, SEEK_SET);

	char* buf = (char*)malloc((*size) * sizeof(char));

	int res = fread(buf, *size, 1, f);
	if (res != 1)
	{
		printf(" | Failed to read from file.\n");
		return NULL;
	}

	printf(" | OK.\n");
	return buf;
}







//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////



int main(int argc, char** argv) {


	ht_opcodes = ht_create();


	char buff[255];
	char buff2[255];
	FILE *fp;

	fp = fopen("tasm1.tab", "r");

	if (fp == NULL)
	{
		printf("Error!");
		exit(1);
	}

	while (fscanf(fp, "%255[^\n]%*c\n", buff) != EOF) {


		char _desc[50];
		char _opcode[50];
		char __size[50];
		int _size = 0;

		int col = 0;


		char *token = strtok(buff, "*\t");
		while (token != NULL) {

			if (col == 0)
				sprintf(_desc, "%s", token);

			else if (col == 1)
				sprintf(_opcode, "%s", token);

			else if (col == 2) {
				sprintf(__size, "%s", token);
				_size = atoi(__size);
			}
			col++;
			token = strtok(NULL, "*\t");
		}

		if (_size >= 1) {
			ht_insert(ht_opcodes, strlwr(_opcode), tasm_opcode_create(_opcode, _desc, _size));
			//printf("%s (%s) = %d\n", _opcode, _desc, _size);
		}
	}

	fclose(fp);

	/*
		char inst

		while (scanf("%20c %20c %d %20c %d", first, last) == 2)
		{
			null_trailing_blanks(first, sizeof(first));
			null_trailing_blanks(last, sizeof(last));
			printf("[%s] [%s]\n", first, last);
		}
		*/


		/*
		sol1_alu_8bit alu3;
		SOL1_BYTE x = 0;// 0xFFFE;
		SOL1_BYTE y = 0;
		SOL1_BYTE alu_op = 0b0000; //0b1001;
		SOL1_BYTE alu_mode = 0x0;

		SOL1_BYTE alu_cf_in_src = 0x01;
		SOL1_BYTE alu_cf_in_inv = 0x01;
		SOL1_BYTE alu_cf_out_inv = 0x01;
		SOL1_BYTE u_cf = 0;
		SOL1_BYTE msw_cf = 0;
		SOL1_BYTE shift_src = 0;
		SOL1_BYTE zbus_out_src = 0;


		alu3.alu_op = alu_op;
		alu3.alu_mode = alu_mode;
		alu3.alu_cf_in_src = alu_cf_in_src;
		alu3.alu_cf_in_inv = alu_cf_in_inv;
		alu3.alu_cf_out_inv = alu_cf_out_inv;


		SOL1_BYTE z_bus = 0;

		SOL1_BYTE alu_cin = 0x00;

		if ((alu_cf_in_src & 0b00000011) == 0x00)
			alu_cin = 1; // AQUI ESTRANho
			//alu_cin = 0;
			//alu_cin = alu_mode;
		else
			if ((alu_cf_in_src & 0b00000011) == 0x01)
				alu_cin = msw_cf;

			else if ((alu_cf_in_src & 0b00000011) == 0x02)
				alu_cin = u_cf;


			else if ((alu_cf_in_src & 0b00000011) == 0x03)
				alu_cin = 0;


		alu_cin = (alu_cin ^ alu_cf_in_inv) & 0b00000001;
		//sol1_alu_8bit_op(alu, x_bus, y_bus, (~alu_cin) & 0b00000001, alu_op, alu_mode); AQUI ESTRANho
		//sol1_alu_8bit_op(alu, x_bus, y_bus, (alu_cin) & 0b00000001, alu_op, alu_mode);

		sol1_alu_4bit alu41;
		sol1_alu_4bit alu42;


		sol1_alu_4bit_op(&alu41, x & 0b00001111, y & 0b00001111, (alu_cin) & 0b00000001, alu_op, alu_mode);

		sol1_alu_4bit_op(&alu42, (x & 0b11110000) >> 4, (y & 0b11110000) >> 4, (alu41.COUT) & 0b00000001, alu_op, alu_mode);

		alu3.A = x; alu3._A = alu3.A;
		alu3.B = y; alu3._B = alu3.B;
		alu3.C = (alu41.alu_output & 0b00001111) | ((alu42.alu_output & 0b00001111) << 4); alu3._C = alu3.C;
		alu3.alu_output = alu3.C;
		alu3.COUT = alu42.COUT;

			//if (x == 0xFF)
				//break;
		//}


		return 1;
		*/
		/*
		sol1_alu_8bit alu2;
		SOL1_BYTE A = 0x01;
		SOL1_BYTE B = 0x01;
		//SOL1_BYTE CIN = 0x01;
		SOL1_BYTE M = 0x01;

		int S = 0;
		for (S = 0; S <= 0xF; S++){
			print_byte_bin(A);
			printf("\t");
			print_byte_bin(B);
			printf("\t");
			print_nibble_bin(S);
			printf(" = ");
			//print_nibble_bin(sol1_alu_8bit_op(&alu2, A, B, CIN, S, M));
			print_nibble_bin(sol1_alu_8bit_op(&alu2, A, B, 0x00, S, M));
			printf(" ");
			if (alu2.EQ == 0x00)
				printf("0");
			else
				printf("1");
			if (alu2.COUT == 0x00)
				printf("0");
			else
				printf("1");
			printf(" | ");
			print_nibble_bin(sol1_alu_8bit_op(&alu2, A, B, 0x01, S, M));
			printf(" ");
			if (alu2.EQ == 0x00)
				printf("0");
			else
				printf("1");
			if (alu2.COUT == 0x00)
				printf("0");
			else
				printf("1");
			printf("\n");
		}
		return 1;
		*/
		/*
		sol1_alu alu;
		void sol1_alu_xor(struct sol1_alu* alu);
		void sol1_alu_or(struct sol1_alu* alu);
		void sol1_alu_and(struct sol1_alu* alu);
		void sol1_alu_not(struct sol1_alu* alu);
		void sol1_alu_shl(struct sol1_alu* alu);
		void sol1_alu_shr(struct sol1_alu* alu);

		void sol1_alu_sum(struct sol1_alu* alu);
		void sol1_alu_sub(struct sol1_alu* alu);


		alu.A = 0xFFFF;// 0b0000000000000000;
		alu.B = 0xFFFF;// 0b0000000000000000;
		alu.C = 0x00;
		alu.F = 0x00;
		alu.CIN = 0x00;

		sol1_alu_xor(&alu);

		printf("A: ");
		print_word_bin(alu.A);

		printf("B: ");
		print_word_bin(alu.B);
		printf("\n");

		printf("C: ");
		print_word_bin(alu.C);
		printf("\n");

		printf("Flags: ");
		print_byte_bin(alu.F);
		printf("\n");


		if (alu.F & 0x01)== 0x01)
			printf("SOL1_ALU_ZERO\n");
		if ((alu.F >> 1) & 0x01)== 0x01)
			printf("SOL1_ALU_EQUAL\n");
		if ((alu.F >> 2) & 0x01)== 0x01)
			printf("SOL1_ALU_LARGER\n");
		if ((alu.F >> 3) & 0x01)== 0x01)
			printf("SOL1_ALU_CARRY_OUT\n");
	*/



	/*
	int opcode = 0;
	for (opcode = 0; opcode <= 0xFF; opcode++) {
		printf("****************************\n");
		printf("%02x\t%s\n", opcode, &sol1_cpu.rom.rom_desc[0x400000 + (opcode * 256)]);

		int cycle = 0;
		for (cycle = 0; cycle < 64; cycle++) {
			if (strlen((const char*)(&sol1_cpu.rom.rom_desc[(256 * 64 * opcode) + (256 * cycle)])) > 0) {
				printf("--- %02d [%02x]\n", cycle, cycle);
				printf("%s\n\n", &sol1_cpu.rom.rom_desc[(256 * 64 * opcode) + (256 * cycle)]);
			}
		}

	}
	*/

	/*
	int opcode = 0;
	for (opcode = 0; opcode < 0xFFFF; opcode++) {
		SOL1_BYTE a = (sol1_cpu.rom.roms[4][opcode] >> 3) & 0b00001111;
		SOL1_BYTE m = (sol1_cpu.rom.roms[4][opcode] >> 7) & 0b00000001;
		if (a != 0x00) {
			print_byte_bin(a);
			printf("   %d", m);
			printf("\n");
		}

	}
	*/

	fa = fopen("File1.txt", "w");
	sol1_cpu sol1_cpu;
	sol1_cpu_init(&sol1_cpu);

	sol1_alu_8bit alu;
	sol1_alu_init(&alu);

	hw_uart_init(&hw_uart);

	hw_rtc_init(&hw_rtc);
	hw_ide_init(&hw_ide);



	int i;
	long size = 0;

	for (i = 0; i < 0x7FFF + 1; i++) {
		sol1_cpu.memory.memchip0[i] = 0x00;
		sol1_cpu.memory.memchip1[i] = 0x00;
	}

	char * buf = loadfile("bios.obj", &size);
	//char * buf = loadfile("bios_paulo.obj", &size);
	if (buf == NULL)
		return 0;

	for (i = 0; i < size; i++) {
		sol1_cpu.memory.memory[i] = buf[i];
	}

	
	hw_ide_load_disk(hw_ide.memory);
	/*
	buf = loadfile("boot.obj", &size);
	//char * buf = loadfile("bios_paulo.obj", &size);
	if (buf == NULL)
		return 0;

	for (i = 0; i < size; i++) {
		hw_ide.memory[i] = buf[i];
	}

	buf = loadfile("kernel.obj", &size);
	//char * buf = loadfile("bios_paulo.obj", &size);
	if (buf == NULL)
		return 0;

	for (i = 0; i < size; i++) {
		hw_ide.memory[512 + i] = buf[i];
	}
	*/
	sol1_cpu.memory.memory[0xFFD7] = 0x00;// IDE IS READY



	/*
				 if(kbhit()){
				int key = getch();
	debugmenu_main(&sol1_cpu);
	*/



	alu.alu_zf = 0x00; // flags do alu
	alu.alu_cf = 0x00;
	alu.alu_of = 0x00;

	alu.alu_final_cf = 0x00;

	alu.alu_op = 0x00;
	alu.alu_mode = 0x00;
	alu.alu_cf_in_src = 0x00;
	alu.alu_cf_in_inv = 0x00;
	alu.alu_cf_out_inv = 0x00;

	alu.alu_a_src = 0x00;
	alu.alu_b_src = 0x00;
	//



	/////


	//SOL1_DWORD u_ad_bus = 0b0000000000000000;


	struct bus bus;


	bus.data_bus = 0b00000000;

	bus.k_bus = 0b00000000; // input pra alu x e y
	bus.w_bus = 0b00000000; // input pra alu x e y
	bus.x_bus = 0b00000000; //alu entrada
	bus.y_bus = 0b00000000; //alu entrada
	//bus.z_bus = 0b00000000; //alu saida


	long runtime_counter = 0;


	if (sol1_cpu.SERVER) {
		hw_tty_start_server();
	}

	//cpu_print(&z80);
	int run = 0;
	int debug = 0;
	while (1) {
		char *input = (char*)malloc(sizeof(char) * 257);

		//sol1_cpu_memory_display(&sol1_cpu);
		//sol1_cpu_display_registers_lite(&sol1_cpu);
		//printf("Flags: "); mswh_flags_desc(&sol1_cpu); printf("\n");
		//printf("Status: "); mswl_status_desc(&sol1_cpu); printf("\n");


		printf(">> ");
		scanf("%s", input);

		if (input[0] != 'n' && input[0] != 'N' &&
			input[0] != 'm' && input[0] != 'M' &&
			input[0] != 'r' && input[0] != 'R' &&
			input[0] != 'b' && input[0] != 'B' &&
			input[0] != 'p' && input[0] != 'P' &&
			input[0] != 's' && input[0] != 'S') {
			size_t numdigits = strlen(input) / 2;
			size_t i;
			for (i = 0; i != numdigits; ++i)
			{
				unsigned char output = 16 * toInt(input[2 * i]) + toInt(input[2 * i + 1]);
				sol1_cpu.memory.memory[i] = output;
				//printf("%x\n", output[i]);
			}

			step = 1;
			sol1_registers_set(sol1_cpu.registers.PCl, sol1_cpu.registers.PCh, 0x00);
			sol1_registers_set(sol1_cpu.memory.MARl, sol1_cpu.memory.MARh, 0x00);


			sol1_cpu_memory_display(&sol1_cpu);
			sol1_cpu_display_registers_lite(&sol1_cpu);
			runtime_counter = 0;
		}
		else if (input[0] == 'n' || input[0] == 'N') {
			debugmenu_main_menu();
			debugmenu_main(&sol1_cpu);
			debug = 1;
		}
		else if (input[0] == 'r' || input[0] == 'R') {

			///apagar
			sol1_cpu.DEBUG_MICROCODE = 0;
			sol1_cpu.DEBUG_REGISTERS = 0;
			sol1_cpu.DEBUG_ALU = 0;
			sol1_cpu.DEBUG_UFLAGS = 0;
			sol1_cpu.DEBUG_UART = 0;
			///
			run = 1;
			step = 0;
			microcodestep = 0;
			printf("\n\n");
			printf("###########################################\n");
			printf("## Running Instructions ###################\n");
			printf("###########################################\n");
		}
		else if (input[0] == 's' || input[0] == 'S') {
			run = 0;
			step = 1;
			microcodestep = 0;
			if (!sol1_cpu.DEBUG_LITE) {
				printf("\n\n");
				printf("###########################################\n");
				printf("## OpCode Step ############################\n");
				printf("###########################################\n");
			}
		}
		else if (input[0] == 'm' || input[0] == 'M') {
			run = 0;
			step = 0;
			microcodestep = 1;
			if (!sol1_cpu.DEBUG_LITE) {
				printf("\n\n");
				printf("###########################################\n");
				printf("# Microcode Step ##########################\n");
				printf("###########################################\n");
				printf("***** REGISTERS\n");
				sol1_cpu_display_registers_lite(&sol1_cpu);
				printf("* IR: %02x\n", sol1_register_8bit_value(sol1_cpu.microcode.IR));
				printf("\n");
			}

		}
		else if (input[0] == 'b' || input[0] == 'B') {
			run = 0;
			step = 1;
			microcodestep = 0;

			sol1_cpu.microcode.u_adder = sol1_cpu.microcode.old_u_ad_bus;
		}
		else if (input[0] == 'p' || input[0] == 'P') {
			sol1_register_8bit_set(sol1_cpu.microcode.IR, 0x00);
			sol1_cpu_reset(&sol1_cpu);
			sol1_memory_reset(&sol1_cpu.memory);
			debug = 1;
		}

		free(input);

		if (debug == 0)
			RunCPU(&sol1_cpu,
				&alu,
				&bus,
				&runtime_counter
			);
		else
			debug = 0;

		oldPC = -1;
		oldOP = -1;

		if (run == 0 && step == 0 && microcodestep == 0)
			sol1_cpu.microcode.u_adder = 0x10; // 0xFE * 64;
		//sol1_microcode.u_ad_bus = 0xFE * 64;
		//sol1_registers_set(sol1_microcode.U_ADDRESSl, sol1_microcode.U_ADDRESSh, sol1_microcode.u_ad_bus);

		/*
		do {
			//cls(hConsole);

			//COORD pos = { 0, 0 };
			//SetConsoleCursorPosition(hConsole, pos);

			cpu_exec(&z80);
			printf("%s", z80.last_op_desc);
			cpu_print(&z80);
			usleep(100 * 1000);
			//sleep(1);
		} while (((z80.registers.IFF & IFF_HALT) != IFF_HALT) && run);
		*/
		//run = 0;
	}


	return 0;
}
