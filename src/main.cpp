#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <chrono>
#ifdef __MINGW32__
#include <time.h>
#include <pthread.h> 
#endif

#include <winsock2.h>

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


#include "utils.h"

#include "sol1_register_8bit.h"

#include "ttl74series.h"

#include <math.h>




SOCKET server;


typedef struct  computer_client {
	SOCKET *client;
	sol1_cpu *sol1_cpu;
};

DWORD WINAPI ClientThread(LPVOID pParam)
{

	computer_client *new_computer_client = (computer_client*)pParam;


	char buff[512];
	int n;
	int x = 0;
	BOOL auth = false;
	sol1_cpu *sol1_cpu = new_computer_client->sol1_cpu;
	SOCKET client = *new_computer_client->client;// (SOCKET)pParam;
	strcpy(buff, "#Server Ready.\r\n");
	send(client, buff, strlen(buff), 0);

	sol1_cpu->memory.memory[0xFF85] = 0xFF;
	while (true)
	{

		if (sol1_cpu->memory.memory[0xFF80] != 0x00 && sol1_cpu->memory.memory[0xFF80] != 0xFF) {
			sol1_cpu->memory.memory[0xFF85] = 0x00;
			char buf_send[2];
			buf_send[0] = sol1_cpu->memory.memory[0xFF80];
			buf_send[1] = '\0';
			send(client, buf_send, strlen(buf_send), 0);
			printf("%c", sol1_cpu->memory.memory[0xFF80]);
			//printf("\n");
			sol1_cpu->memory.memory[0xFF80] = 0;
			sol1_cpu->memory.memory[0xFF85] = 0xFF;
		}
		if (x == 0)
			n = recv(client, buff, 512, 0);

		if (n > 0) {

			sol1_cpu->memory.memory[0xFF80] = buff[x];
			sol1_cpu->memory.memory[0xFF85] = 0x00;
			if (x < n)
				x++;
			else {
				n = 0;
				x = 0;
				memset(buff, 0, 512 * (sizeof buff[0]));
				sol1_cpu->memory.memory[0xFF85] = 0xFF;
			}

		}

	}

	closesocket(client);
	return 0;

}

DWORD WINAPI MTServerThread(LPVOID pParam)
{

	WSADATA wsaData;
	sockaddr_in local;
	int wsaret = WSAStartup(0x101, &wsaData);
	if (wsaret != 0)
	{
		return 0;
	}
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = INADDR_ANY;
	local.sin_port = htons((u_short)20248);
	server = socket(AF_INET, SOCK_STREAM, 0);
	if (server == INVALID_SOCKET)
	{
		return 0;
	}
	if (bind(server, (sockaddr*)&local, sizeof(local)) != 0)
	{
		return 0;
	}
	if (listen(server, 10) != 0)
	{
		return 0;
	}

	SOCKET client;
	sockaddr_in from;
	int fromlen = sizeof(from);

	while (true)
	{
		client = accept(server,
			(struct sockaddr*)&from, &fromlen);

		u_long mode = 1;  // 1 to enable non-blocking socket
		ioctlsocket(client, FIONBIO, &mode);

		computer_client new_computer_client;
		new_computer_client.client = &client;
		new_computer_client.sol1_cpu = (sol1_cpu*)pParam;

		DWORD tid;
		HANDLE myHandle = CreateThread(0, 0, ClientThread, &new_computer_client, 0, &tid);
	}

	return 0;
}


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





SOL1_BYTE bus_tristate(sol1_cpu *sol1_cpu) {

	if (get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWl), MSW_HALT)) // GAMBIARRA ENQUANTO SEM INTERRUPCOES
		sol1_register_8bit_set(sol1_cpu->registers.MSWl, sol1_register_8bit_value(sol1_cpu->registers.MSWl) & 0b11101110);

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

			SOL1_BYTE page_tbl_addr_source = 0x01; ////VERIFICAR

			SOL1_MWORD ptb_mem_addr = ((sol1_register_8bit_value(sol1_cpu->memory.MARh) >> 3) & 0b00011111);
			if (page_tbl_addr_source == 0x01)
				ptb_mem_addr = ptb_mem_addr | sol1_register_8bit_value(sol1_cpu->registers.PTB) << 5;

			if (sol1_cpu->microcode.mccycle.mdr_to_pagtbl_en == 0x0)
				address_bus = (sol1_cpu->memory.memchip1[ptb_mem_addr] << 8) | sol1_cpu->memory.memchip0[ptb_mem_addr];
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
			break;
		case 0x07:
			w_bus = sol1_register_8bit_value(sol1_cpu->registers.Dh);
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
	mc_seq_update(sol1_cpu,
		alu);




	if (!(sol1_cpu->rom.bkpt_opcode == 0 && sol1_cpu->rom.bkpt_cycle == 0) &&
		((sol1_cpu->microcode.u_ad_bus / 64) == sol1_cpu->rom.bkpt_opcode &&
		(sol1_cpu->microcode.u_ad_bus % 64) == sol1_cpu->rom.bkpt_cycle))
	{
		printf(" Microcode Breakpoint | Starting Opcode/Cycle:%02x%02x.\n", sol1_cpu->rom.bkpt_opcode, sol1_cpu->rom.bkpt_cycle);
		debugmenu_main(sol1_cpu);
	}
	else if ((sol1_cpu->BKPT != 0xFFFF) && (sol1_registers_value(sol1_cpu->memory.MARl, sol1_cpu->memory.MARh) == sol1_cpu->BKPT
		|| sol1_registers_value(sol1_cpu->registers.PCl, sol1_cpu->registers.PCh) == sol1_cpu->BKPT))
	{
		printf(" Memory Breakpoint | Starting Address:%04x.\n", sol1_cpu->BKPT);
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

	SOL1_BYTE inIC18A = IC_74LS32(0x00, sol1_cpu->microcode.mccycle.int_pending & 0b00000001);
	SOL1_BYTE inIC18B = IC_74LS32(inIC18A & 0b00000001, sol1_cpu->microcode.mccycle.dma_req);
	sol1_cpu->microcode.mccycle.any_interruption = inIC18B & 0b00000001;

	SOL1_BYTE inIC22A = IC_74LS08(sol1_cpu->microcode.mccycle.int_request & 0b00000001, get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWl), MSW_INTERRUPT_ENABLE));
	sol1_cpu->microcode.mccycle.int_pending = inIC22A & 0b00000001;

	SOL1_BYTE inIC18C = IC_74LS32(sol1_cpu->microcode.mccycle.force_user_ptb & 0b00000001, get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWl), MSW_CPU_MODE));
	sol1_cpu->microcode.mccycle.page_table_addr_src = inIC18C & 0b00000001;






	////////////////////////////////////////////////////////////////////////////

	if (get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWl), MSW_PAGING_EN) == 0x00) {//|| buffer_mem_io(sol1_cpu, sol1_cpu->microcode.mccycle.) == 0x01) {
		if (buffer_rd(sol1_cpu) == 0x00) {
			unsigned long mem_addr = read_address_bus(sol1_cpu);

			bus->data_bus = sol1_cpu->memory.memory[mem_addr];// bus->address_bus & 0b0111111111111111];
			//if (mem_addr >= 0xFF80) 
			//if (mem_addr >= 0xF7FF) 
			if (mem_addr == 0xF7FF)
				if (DEBUG_RDMEM)
					printf("] READ RAM [%x] = %x\n", mem_addr, bus->data_bus);
		}
	}
	else {
		if (buffer_rd(sol1_cpu) == 0x00) {
			unsigned long mem_addr = read_address_bus(sol1_cpu);
			//bus->data_bus = sol1_cpu->memory.memory[mem_addr];// bus->address_bus & 0b0111111111111111];
			bus->data_bus = sol1_cpu->memory.memchip_test[mem_addr];// bus->address_bus & 0b0111111111111111];
			//if (mem_addr >= 0xFF80) 
			//if (mem_addr >= 0xF7FF) 
			if (DEBUG_RDMEM)
				printf("] READ OTHER RAM [%x] = %x\n", mem_addr, bus->data_bus);

		}

	}
	//else if (sol1_cpu->microcode.mccycle.buffer_wr == 0x00)
	//	sol1_cpu->memory.memory[bus->address_bus & 0b0111111111111111] = bus->data_bus;
//}

////////////////////////////////////////////////////////////////////////////
// W = X
	bus->w_bus = refresh_w_bus(sol1_cpu->microcode.mccycle.panel_regsel,
		alu->alu_a_src, sol1_cpu->microcode.mccycle.display_reg_load,
		sol1_cpu->microcode.mccycle.int_vector, sol1_cpu->microcode.mccycle.int_mask, sol1_cpu->microcode.mccycle.int_status,
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

	SOL1_BYTE alu_result = ALU_EXEC(alu, bus->x_bus, bus->y_bus,
		alu->alu_op,
		alu->alu_mode,
		alu->alu_cf_in_src,
		alu->alu_cf_in_inv,
		alu->alu_cf_out_inv,
		sol1_cpu->microcode.u_cf,
		get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWh), MSW_CF),
		sol1_cpu->microcode.mccycle.shift_src,
		sol1_cpu->microcode.mccycle.zbus_out_src);

	////////////////////////////////////////////////////////////////////////////
	sol1_u_flags(alu, &sol1_cpu->microcode, sol1_cpu->microcode.mccycle.imm, bus->z_bus);
	if (DEBUG_UFLAGS) {
		printf("***** U_FLAGS\n");
		sol1_microcode_display_u_flags(&sol1_cpu->microcode);
		printf("\n");
	}
	bus->z_bus = alu_result;

	//#######################
	if (get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWl), MSW_PAGING_EN) == 0x00) {//|| buffer_mem_io(sol1_cpu, sol1_cpu->microcode.mccycle.) == 0x01) {
		if (buffer_wr(sol1_cpu) == 0x00) {
			unsigned long mem_addr = read_address_bus(sol1_cpu);
			//if (mem_addr >= 0xFF80)
			//if (mem_addr > 0x0618)
				//if (mem_addr <= 0x0618)
			if (DEBUG_WRMEM)
				printf("] WRITE RAM [%x] = %x\n", mem_addr, bus->data_bus);
			//}
			//sol1_cpu->memory.memory[read_address_bus(sol1_cpu)] = bus->data_bus;
			sol1_cpu->memory.memory[mem_addr] = bus->data_bus;
		}
	}
	else {
		if (buffer_wr(sol1_cpu) == 0x00) {
			unsigned long mem_addr = read_address_bus(sol1_cpu);
			//if (mem_addr >= 0xFF80)
			//if (mem_addr > 0x0618)
				//if (mem_addr <= 0x0618)
			if (DEBUG_WRMEM)
				printf("] WRITE OTHER RAM [%x] = %x\n", mem_addr, bus->data_bus);
			//}
			//sol1_cpu->memory.memory[read_address_bus(sol1_cpu)] = bus->data_bus;
			sol1_cpu->memory.memchip_test[mem_addr] = bus->data_bus;
			//sol1_cpu->memory.memory[mem_addr] = bus->data_bus;
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

		if (get_byte_bit(oldStatus, MSW_CPU_MODE) != get_byte_bit(sol1_register_8bit_value(sol1_cpu->registers.MSWl), MSW_CPU_MODE))
		{
			SOL1_BYTE oldSP = sol1_register_8bit_value(sol1_cpu->registers.SPl);
			sol1_register_8bit_set(sol1_cpu->registers.SPl, sol1_register_8bit_value(sol1_cpu->registers.SSPl));
			sol1_register_8bit_set(sol1_cpu->registers.SSPl, oldSP);

			oldSP = sol1_register_8bit_value(sol1_cpu->registers.SPh);
			sol1_register_8bit_set(sol1_cpu->registers.SPh, sol1_register_8bit_value(sol1_cpu->registers.SSPh));
			sol1_register_8bit_set(sol1_cpu->registers.SSPh, oldSP);
		}

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

	if (sol1_cpu->microcode.mccycle.gh_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.Gh, bus->z_bus);
	if (sol1_cpu->microcode.mccycle.gl_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.Gl, bus->z_bus);

	//Pointer Registers
	if (sol1_cpu->microcode.mccycle.bph_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.BPh, bus->z_bus);
	if (sol1_cpu->microcode.mccycle.bpl_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.BPl, bus->z_bus);

	if (sol1_cpu->microcode.mccycle.sph_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.SPh, bus->z_bus);
	if (sol1_cpu->microcode.mccycle.spl_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.SPl, bus->z_bus);

	if (sol1_cpu->microcode.mccycle.ssph_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.SSPh, bus->z_bus);
	if (sol1_cpu->microcode.mccycle.sspl_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.SSPl, bus->z_bus);


	//Index Registers
	if (sol1_cpu->microcode.mccycle.sih_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.SIh, bus->z_bus);
	if (sol1_cpu->microcode.mccycle.sil_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.SIl, bus->z_bus);

	if (sol1_cpu->microcode.mccycle.dih_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.DIh, bus->z_bus);
	if (sol1_cpu->microcode.mccycle.dil_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.DIl, bus->z_bus);

	if (sol1_cpu->microcode.mccycle.pch_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.PCh, bus->z_bus);
	if (sol1_cpu->microcode.mccycle.pcl_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.PCl, bus->z_bus);

	if (sol1_cpu->microcode.mccycle.tdrh_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.TDRh, bus->z_bus);
	if (sol1_cpu->microcode.mccycle.tdrl_wrt == 0x00) sol1_register_8bit_set(sol1_cpu->registers.TDRl, bus->z_bus);

	if (sol1_cpu->microcode.mccycle.ptb_wrt == 0x00)
		sol1_register_8bit_set(sol1_cpu->registers.PTB, bus->z_bus);
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
		SOL1_BYTE page_tbl_addr_source = 0x01; ////VERIFICAR

		SOL1_MWORD ptb_mem_addr = ((sol1_register_8bit_value(sol1_cpu->memory.MARh) >> 3) & 0b00011111);
		if (page_tbl_addr_source == 0x01)
			ptb_mem_addr = ptb_mem_addr | sol1_register_8bit_value(sol1_cpu->registers.PTB) << 5;

		if (sol1_cpu->microcode.mccycle.mdr_to_pagtbl_en == 0x1) {
			sol1_cpu->memory.memchip0[ptb_mem_addr] = sol1_register_8bit_value(sol1_cpu->memory.MARl);
			sol1_cpu->memory.memchip1[ptb_mem_addr] = sol1_register_8bit_value(sol1_cpu->memory.MARh);
		}

		else {
			sol1_cpu->memory.memchip0[ptb_mem_addr] = 0;
			sol1_cpu->memory.memchip1[ptb_mem_addr] = 0;
		}
	}


	////////////////////////////////////////////////////////////////////////////

	if (DEBUG_BUSES) {
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

	if (DEBUG_MEMORY) {
		//printf("***** MEMORY\n");
		sol1_cpu_memory_display(sol1_cpu);
	}

	if (DEBUG_REGISTERS) {
		printf("***** REGISTERS\n");
		sol1_cpu_display_registers_lite(sol1_cpu);
		printf("* IR: %02x\n", sol1_register_8bit_value(sol1_cpu->microcode.IR));
		printf("\n");
	}


	//CLOCK HIGH

	sol1_u_adder(&sol1_cpu->microcode, sol1_cpu->microcode.mccycle.next, sol1_cpu->microcode.mccycle.final_condition);
	if (DEBUG_UADDER) {
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
	else if ((sol1_cpu->BKPT != 0xFFFF) && (sol1_registers_value(sol1_cpu->memory.MARl, sol1_cpu->memory.MARh) == sol1_cpu->BKPT
		|| sol1_registers_value(sol1_cpu->registers.PCl, sol1_cpu->registers.PCh) == sol1_cpu->BKPT))
	{
		printf(" Memory Breakpoint | Address:%04x Finished.\n", sol1_cpu->BKPT);
		debugmenu_main(sol1_cpu);
	}

	//if (MUX(sol1_cpu->microcode.mccycle.) == 0x02 && step == 0)
		//return;



	bus->data_bus = 0;
	bus->k_bus = 0;
	bus->w_bus = 0;
	bus->x_bus = 0;
	bus->y_bus = 0;
	bus->z_bus = 0;




}


void RunCPU(
	sol1_cpu *sol1_cpu,
	sol1_alu_8bit *alu,
	struct bus *bus,
	long *runtime_counter
) {

	while (1) {
		
		mc_sequencer(sol1_cpu, alu, bus, runtime_counter);

		if (!SERVER) {


			if (sol1_cpu->memory.memory[0xFF80] != 0x00 && sol1_cpu->memory.memory[0xFF80] != 0xFF) {
				//printf("(%02x) %c", sol1_cpu->memory.memory[0xFF80], sol1_cpu->memory.memory[0xFF80]);
				printf("%c", sol1_cpu->memory.memory[0xFF80]);
				//printf("\n");
				//sol1_cpu->memory.memory[0xFF80] = 0x00;
				sol1_cpu->memory.memory[0xFF80] = 0;
			}

			if (kbhit()) {
				char dddd = getch();
				if (dddd == 0x1b)
					return;


				sol1_cpu->memory.memory[0xFF80] = dddd;

				if (dddd == 13)

					printf("\n");
				sol1_cpu->memory.memory[0xFF85] = 0xFF;
			}
		}


		/*
		printf("%02x(%d) | ", sol1_cpu->memory.memory[0xFFD0], sol1_cpu->memory.memory[0xFFD0]);
		printf("%02x(%d) | ", sol1_cpu->memory.memory[0xFFD1], sol1_cpu->memory.memory[0xFFD1]);
		printf("%02x(%d) | ", sol1_cpu->memory.memory[0xFFD2], sol1_cpu->memory.memory[0xFFD2]);
		printf("%02x(%d) | ", sol1_cpu->memory.memory[0xFFD3], sol1_cpu->memory.memory[0xFFD3]);
		printf("%02x(%d) | ", sol1_cpu->memory.memory[0xFFD4], sol1_cpu->memory.memory[0xFFD4]);
		printf("%02x(%d) | ", sol1_cpu->memory.memory[0xFFD5], sol1_cpu->memory.memory[0xFFD5]);
		printf("%02x(%d) | ", sol1_cpu->memory.memory[0xFFD6], sol1_cpu->memory.memory[0xFFD6]);
		printf("%02x(%d) | ", sol1_cpu->memory.memory[0xFFD7], sol1_cpu->memory.memory[0xFFD7]);

		printf("\n");
		*/

		//if (sol1_registers_value(sol1_cpu->registers.Al, sol1_cpu->registers.Ah) == 0xF7FF) return;

		//if (sol1_registers_value(sol1_cpu->registers.Dl, sol1_cpu->registers.Dh) == 0x00e8) return;

		/*
		print_byte_bin(sol1_cpu->memory.memory[0xFF80]);
		printf(" | ");
		print_byte_bin(sol1_cpu->memory.memory[0xFF81]);
		printf(" | ");
		print_byte_bin(sol1_cpu->memory.memory[0xFF82]);
		printf(" | ");
		print_byte_bin(sol1_cpu->memory.memory[0xFF83]);
		printf(" | ");
		print_byte_bin(sol1_cpu->memory.memory[0xFF85]);
		printf("\n");
		*/

		if (DEBUG_LITE && ((sol1_cpu->microcode.u_ad_bus / 64) != oldOP || sol1_registers_value(sol1_cpu->registers.PCl, sol1_cpu->registers.PCh) != oldPC)) {

			SOL1_BYTE cur_opcode = (sol1_cpu->microcode.u_ad_bus / 64);
			SOL1_BYTE pc_opcode = sol1_cpu->memory.memory[sol1_registers_value(sol1_cpu->registers.PCl, sol1_cpu->registers.PCh)];

			char str[100];
			sprintf(str, "# Opcode=%02x:%s", cur_opcode, &sol1_cpu->rom.rom_desc[0x400000 + (cur_opcode * 256)]);
			printf("%s", rightpad(str, 40));

			printf(" | ");
			printf("PC: % 04x", sol1_registers_value(sol1_cpu->registers.PCl, sol1_cpu->registers.PCh));
			printf(" | ");
			printf("@PC=%02x:%s\n", pc_opcode, &sol1_cpu->rom.rom_desc[0x400000 + (pc_opcode * 256)]);

			oldOP = cur_opcode;
			oldPC = sol1_registers_value(sol1_cpu->registers.PCl, sol1_cpu->registers.PCh);
		}

		if (MUX(&sol1_cpu->microcode) == 0x02 && step == 1) {
			/*
			print_byte_bin(sol1_cpu->memory.memory[0xFF80]);
			printf(" | ");
			print_byte_bin(sol1_cpu->memory.memory[0xFF81]);
			printf(" | ");
			print_byte_bin(sol1_cpu->memory.memory[0xFF82]);
			printf(" | ");
			print_byte_bin(sol1_cpu->memory.memory[0xFF83]);
			printf(" | ");
			print_byte_bin(sol1_cpu->memory.memory[0xFF85]);
			*/

			if (!DEBUG_LITE) {
				printf("###########################################\n");
				printf("## End OpStep on Opcode/Cycle:%02x%02x. #######\n", (sol1_cpu->microcode.u_ad_bus / 64), (sol1_cpu->microcode.u_ad_bus % 64));
				printf("###########################################\n");
			}
			return;
		}
		else if (step == 1 && !DEBUG_LITE) {
			printf("###########################################\n");
		}
		else if (microcodestep == 1) {
			/*
			print_byte_bin(sol1_cpu->memory.memory[0xFF80]);
			printf(" | ");
			print_byte_bin(sol1_cpu->memory.memory[0xFF81]);
			printf(" | ");
			print_byte_bin(sol1_cpu->memory.memory[0xFF82]);
			printf(" | ");
			print_byte_bin(sol1_cpu->memory.memory[0xFF83]);
			printf(" | ");
			print_byte_bin(sol1_cpu->memory.memory[0xFF85]);
			*/
			if (!DEBUG_LITE) {
				printf("###########################################\n");
				printf("## End MicroStep on Opcode/Cycle:%02x%02x. ####\n", (sol1_cpu->microcode.u_ad_bus / 64), (sol1_cpu->microcode.u_ad_bus % 64));
				printf("###########################################\n");
			}
			return;
		}


	}
	sol1_cpu->microcode.mccycle.next = 0x00;
}

int main(int argc, char** argv) {
	/*
	sol1_alu_8bit alu3;
	SOL1_BYTE x = 0;// 0xFFFE;
	SOL1_BYTE y = 0;
	SOL1_BYTE alu_op = 0x09; //0b1001;
	SOL1_BYTE alu_mode = 0x0;

	SOL1_BYTE alu_cf_in_src = 0x01;
	SOL1_BYTE alu_cf_in_inv = 0x01;
	SOL1_BYTE alu_cf_out_inv = 0x01;
	SOL1_BYTE u_cf = 1;
	SOL1_BYTE msw_cf = 0;
	SOL1_BYTE shift_src = 0;
	SOL1_BYTE zbus_out_src = 0;


	//for (x = 0; x <= 0xFF; x++) {
		SOL1_BYTE r = ALU_EXEC(&alu3, x, y,
			alu_op,
			alu_mode,
			alu_cf_in_src,
			alu_cf_in_inv,
			alu_cf_out_inv,
			u_cf, msw_cf, shift_src, zbus_out_src);

		printf("* ");
		printf("[%01d] %d (%02x)\t[", alu_mode, x, x);
		print_nibble_bin(alu_op);

		printf("]\t%d (%02x) = %d (%02x)\n", y, y, r, r);
		printf("* EQ="); print_nibble_bin(alu3.EQ); printf(" | ");	printf("F="); print_byte_bin(alu3.F);

		if (alu3.F == 0b00001000) printf(" F_cout");
		if (alu3.F == 0b00000100) printf(" F_lg");
		if (alu3.F == 0b00000010) printf(" F_eq");
		if (alu3.F == 0b00000001) printf(" F_zf");

		printf("\n");
		printf("* Flags: ");

		if (alu3.alu_zf != 0x00) printf("alu_zf ");
		if (alu3.alu_cf != 0x00) printf("alu_cf ");
		if (alu3.alu_of != 0x00) printf("alu_of ");
		if (alu3.alu_final_cf != 0x00) printf("alu_final_cf ");
		printf("\n");

		if (alu3.alu_cf_in_src != 0x00) printf("alu_cf_in_src ");
		if (alu3.alu_cf_in_inv != 0x00) printf("alu_cf_in_inv ");
		if (alu3.alu_cf_out_inv != 0x00) printf("alu_cf_out_inv ");

		if (alu3.alu_a_src != 0x00) printf("alu_a_src ");
		if (alu3.alu_b_src != 0x00) printf("alu_b_src ");
		printf("\n");
		printf("-----------------------------\n");

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


	sol1_cpu sol1_cpu;
	sol1_cpu_init(&sol1_cpu);

	sol1_alu_8bit alu;
	sol1_alu_init(&alu);


	//char *filename = "bios.obj";
	char *filename = "bios_paulo.obj";
	printf("The filename to load is: %s", filename);

	FILE* f = fopen(filename, "rb");
	if (!f)
	{
		printf(" | Failed to open the file.\n");
		return 0;
	}

	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	fseek(f, 0, SEEK_SET);

	char* buf = (char*)malloc(size * sizeof(char));

	int res = fread(buf, size, 1, f);
	if (res != 1)
	{
		printf(" | Failed to read from file.\n");
		return 0;
	}




	int i;
	for (i = 0; i < 0x7FFF + 1; i++) {
		sol1_cpu.memory.memchip0[i] = 0x00;
		sol1_cpu.memory.memchip1[i] = 0x00;
	}

	for (i = 0; i < size; i++) {
		//memchip0[i] = buf[i];
		sol1_cpu.memory.memory[i] = buf[i];
	}

	printf(" | OK.\n");

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
	bus.z_bus = 0b00000000; //alu saida


	long runtime_counter = 0;


	if (SERVER) {
		DWORD tid;
		HANDLE myHandle = CreateThread(0, 0, MTServerThread, &sol1_cpu, 0, &tid);
	}
	else
		sol1_cpu.memory.memory[0xFF85] = 0xFF;

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
			printf("Flags: "); mswh_flags_desc(&sol1_cpu); printf("\n");
			printf("Status: "); mswl_status_desc(&sol1_cpu); printf("\n");
			runtime_counter = 0;
		}
		else if (input[0] == 'n' || input[0] == 'N') {
			debugmenu_main_menu();
			debugmenu_main(&sol1_cpu);
			debug = 1;
		}
		else if (input[0] == 'r' || input[0] == 'R') {
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
			if (!DEBUG_LITE) {
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
			if (!DEBUG_LITE) {
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
