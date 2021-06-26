#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#if defined(__linux__) || defined(__MINGW32__)
#include <time.h>
#else
#include <chrono>
using namespace std::chrono;

#endif

#include "config.h"
#include "sol1_computer.h"
#include "sol1_bus.h"
#include "debugmenu_main.h"

#include "utils.h"

#include <math.h>


#ifdef _MSC_VER     

#include <windows.h>
/* Windows sleep in 100ns units */
BOOLEAN nanosleep(LONGLONG ns) {
	/* Declarations */
	HANDLE timer;	/* Timer handle */
	LARGE_INTEGER li;	/* Time defintion */
	/* Create timer */
	if (!(timer = CreateWaitableTimer(NULL, TRUE, NULL)))
		return FALSE;
	/* Set timer properties */
	li.QuadPart = -ns;
	if (!SetWaitableTimer(timer, &li, 0, NULL, NULL, FALSE)) {
		CloseHandle(timer);
		return FALSE;
	}
	/* Start & wait for timer */
	WaitForSingleObject(timer, INFINITE);
	/* Clean resources */
	CloseHandle(timer);
	/* Slept without problems */
	return TRUE;
}
#endif

/// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//boot sequence: bios, boot, kernel, shell
/// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

int step = 0;
int microcodestep = 0;
SOL1_MWORD oldPC = -1;
SOL1_BYTE oldOP = -1;

SOL1_BYTE last_opcode = 0xFF;

FILE *fa;


SOL1_BYTE SOL1_COMPUTER::get_current_opcode() {
	return (this->cpu.microcode.u_ad_bus / 64);
}

SOL1_BYTE SOL1_COMPUTER::get_current_opcode_cycle() {
	return  (this->cpu.microcode.u_ad_bus % 64);
}



void SOL1_COMPUTER::disassembly_current_opcode() {
	SOL1_BYTE current_opcode = get_current_opcode();
	if (last_opcode != current_opcode) {
		char temp[4];
		char line[255];

		sprintf(temp, "%02x", current_opcode);


		if (this->ht_opcodes.find(temp) != this->ht_opcodes.end()) {
			Tasm_Opcode tt = this->ht_opcodes[temp];
			SOL1_MWORD memADDR = SOL1_REGISTERS::value(this->cpu.registers.PCl, this->cpu.registers.PCh);

			sprintf(line, "    %04x]\t%02x:%s", (int)(memADDR - (strlen(temp) / 2)), current_opcode, tt.desc.c_str());

			if (tt.size > 1) {

				int i = 0;
				sprintf(line + strlen(line), " (");

				for (i = tt.size - 2; i >= 0; i--) {
					if (i != tt.size - 2)
						sprintf(line + strlen(line), " %02x", this->cpu.get_current_memory()[memADDR + i]);
					else
						sprintf(line + strlen(line), "%02x", this->cpu.get_current_memory()[memADDR + i]);
				}
				sprintf(line + strlen(line), ")");
			}

			sprintf(line + strlen(line), "\n");

			char str_out[255];
			save_to_log(str_out, fa, line);
			//this->hw_tty.print(str_out);
		}
		/*
		else if (current_opcode == 0 && sol1_registers_value(sol1_cpu.registers.PCl, sol1_cpu.registers.PCh) == 0)
		{
			save_to_log("########################\n");
			save_to_log("# STARTING - RESET CPU #\n");
			save_to_log("########################\n");

			sprintf(, "RESTARTING ...pressione uma tecla para continuar...\n");
			getch();
		}
		*/
		last_opcode = current_opcode;
	}
}

//////////////////////////
//////////////////////////
//////////////////////////
//////////////////////////
//////////////////////////

void SOL1_COMPUTER::refresh_int() {

	SOL1_BYTE int_req_0 = 0x00;
	SOL1_BYTE int_req_1 = 0x00;
	SOL1_BYTE int_req_2 = 0x00;
	SOL1_BYTE int_req_3 = 0x00;
	SOL1_BYTE int_req_4 = 0x00;
	SOL1_BYTE int_req_5 = 0x00;
	SOL1_BYTE int_req_6 = 0x00;
	SOL1_BYTE int_req_7 = 0x00;

	SOL1_BYTE not_clear_all_ints = get_byte_bit(~this->cpu.microcode.mccycle.clear_all_ints, 0x00);

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


	if (this->cpu.microcode.mccycle.int_ack == 0x01) {
		int_vector_0 = get_byte_bit(~get_byte_bit(this->cpu.microcode.mccycle.int_vector, 0), 0);
		int_vector_1 = get_byte_bit(~get_byte_bit(this->cpu.microcode.mccycle.int_vector, 1), 0);
		int_vector_2 = get_byte_bit(~get_byte_bit(this->cpu.microcode.mccycle.int_vector, 2), 0);
		int_vector_3 = get_byte_bit(~get_byte_bit(this->cpu.microcode.mccycle.int_vector, 3), 0);
		int_vector_4 = get_byte_bit(~get_byte_bit(this->cpu.microcode.mccycle.int_vector, 4), 0);
		int_vector_5 = get_byte_bit(~get_byte_bit(this->cpu.microcode.mccycle.int_vector, 5), 0);
		int_vector_6 = get_byte_bit(~get_byte_bit(this->cpu.microcode.mccycle.int_vector, 6), 0);
		int_vector_7 = get_byte_bit(~get_byte_bit(this->cpu.microcode.mccycle.int_vector, 7), 0);
	}

	SOL1_BYTE int_clr_0 = (int_vector_0 & not_clear_all_ints);
	SOL1_BYTE int_clr_1 = (int_vector_1 & not_clear_all_ints);
	SOL1_BYTE int_clr_2 = (int_vector_2 & not_clear_all_ints);
	SOL1_BYTE int_clr_3 = (int_vector_3 & not_clear_all_ints);
	SOL1_BYTE int_clr_4 = (int_vector_4 & not_clear_all_ints);
	SOL1_BYTE int_clr_5 = (int_vector_5 & not_clear_all_ints);
	SOL1_BYTE int_clr_6 = (int_vector_6 & not_clear_all_ints);
	SOL1_BYTE int_clr_7 = (int_vector_7 & not_clear_all_ints);


	if (int_clr_0 == 0x01 && check_byte_bit(this->cpu.microcode.mccycle.int_req, 0))
		int_status_0 = get_byte_bit(~get_byte_bit(this->cpu.microcode.mccycle.int_status, 0), 0);

	if (int_clr_1 == 0x01 && check_byte_bit(this->cpu.microcode.mccycle.int_req, 1))
		int_status_1 = get_byte_bit(~get_byte_bit(this->cpu.microcode.mccycle.int_status, 1), 0);

	if (int_clr_2 == 0x01 && check_byte_bit(this->cpu.microcode.mccycle.int_req, 2))
		int_status_2 = get_byte_bit(~get_byte_bit(this->cpu.microcode.mccycle.int_status, 2), 0);

	if (int_clr_3 == 0x01 && check_byte_bit(this->cpu.microcode.mccycle.int_req, 3))
		int_status_3 = get_byte_bit(~get_byte_bit(this->cpu.microcode.mccycle.int_status, 3), 0);

	if (int_clr_4 == 0x01 && check_byte_bit(this->cpu.microcode.mccycle.int_req, 4))
		int_status_4 = get_byte_bit(~get_byte_bit(this->cpu.microcode.mccycle.int_status, 4), 0);

	if (int_clr_5 == 0x01 && check_byte_bit(this->cpu.microcode.mccycle.int_req, 5))
		int_status_5 = get_byte_bit(~get_byte_bit(this->cpu.microcode.mccycle.int_status, 5), 0);

	if (int_clr_6 == 0x01 && check_byte_bit(this->cpu.microcode.mccycle.int_req, 6))
		int_status_6 = get_byte_bit(~get_byte_bit(this->cpu.microcode.mccycle.int_status, 6), 0);

	if (int_clr_7 == 0x01 && check_byte_bit(this->cpu.microcode.mccycle.int_req, 7))
		int_status_7 = get_byte_bit(~get_byte_bit(this->cpu.microcode.mccycle.int_status, 7), 0);



	this->cpu.microcode.mccycle.int_status = (int_status_7 << 7) | (int_status_6 << 6) | (int_status_5 << 5) | (int_status_4 << 4) | (int_status_3 << 3) | (int_status_2 << 2) | (int_status_1 << 1) | (int_status_0 << 0);


	if (this->cpu.microcode.mccycle.int_request == 0x00 && this->cpu.microcode.mccycle.int_vector_wrt == 0x00) {

		SOL1_BYTE iii = ~(get_byte_bit(~(int_status_7 & get_byte_bit(this->cpu.registers.INT_MASKS.value(), 7)), 0)) |
			~(get_byte_bit(~(int_status_6 & get_byte_bit(this->cpu.registers.INT_MASKS.value(), 6)), 0) << 1) |
			~(get_byte_bit(~(int_status_5 & get_byte_bit(this->cpu.registers.INT_MASKS.value(), 5)), 0) << 2) |
			~(get_byte_bit(~(int_status_4 & get_byte_bit(this->cpu.registers.INT_MASKS.value(), 4)), 0) << 3) |
			~(get_byte_bit(~(int_status_3 & get_byte_bit(this->cpu.registers.INT_MASKS.value(), 3)), 0) << 4) |
			~(get_byte_bit(~(int_status_2 & get_byte_bit(this->cpu.registers.INT_MASKS.value(), 2)), 0) << 5) |
			~(get_byte_bit(~(int_status_1 & get_byte_bit(this->cpu.registers.INT_MASKS.value(), 1)), 0) << 6) |
			~(get_byte_bit(~(int_status_0 & get_byte_bit(this->cpu.registers.INT_MASKS.value(), 0)), 0) << 6);

		this->cpu.microcode.mccycle.int_vector = iii;
	}

}



SOL1_BYTE  SOL1_COMPUTER::buffer_rd() {
	return this->bus.bus_rd(this->cpu.registers, this->cpu.microcode.mccycle.rd);
}

SOL1_BYTE SOL1_COMPUTER::buffer_wr() {
	return this->bus.bus_wr(this->cpu.registers, this->cpu.microcode.mccycle.wr);
}

SOL1_BYTE SOL1_COMPUTER::buffer_mem_io() {
	return this->cpu.microcode.mccycle.memory_io;
}


unsigned long SOL1_COMPUTER::read_address_bus() {
	unsigned long address_bus = 0x00;

	if (this->bus.bus_tristate(this->cpu.registers) == 0x00)

		if (!check_byte_bit(this->cpu.registers.MSWl.value(), MSWl_PAGING_EN)) {
			address_bus = (this->cpu.registers.MARh.value() << 8) | this->cpu.registers.MARl.value();

			this->cpu.microcode.mccycle.memory_io = 0;
			this->cpu.microcode.mccycle.page_present = 0;
			this->cpu.microcode.mccycle.page_writable = 0;
		}
		else {
			//this->cpu.DEBUG_WRMEM = 1;
			//this->cpu.DEBUG_RDMEM = 1;

			SOL1_MWORD ptb_mem_addr = ((this->cpu.registers.MARh.value() >> 3) & 0b00011111);

			if (this->cpu.microcode.page_table_addr_src(this->cpu.registers) == 0x01)
				ptb_mem_addr = ptb_mem_addr | (this->cpu.registers.PTB.value() << 5);

			if (this->cpu.microcode.mccycle.mdr_to_pagtbl_en == 0x0)
				address_bus = ((this->cpu.memory.memchip1[ptb_mem_addr] & 0b00000011) << 8) | this->cpu.memory.memchip0[ptb_mem_addr];
			else
				address_bus = (this->cpu.registers.MDRh.value() << 8) | this->cpu.registers.MDRl.value();

			address_bus = (address_bus << 11) | ((this->cpu.registers.MARh.value() & 0b00000111) << 8) | this->cpu.registers.MARl.value();

			this->cpu.microcode.mccycle.memory_io = get_byte_bit(this->cpu.memory.memchip1[ptb_mem_addr], 3);
			this->cpu.microcode.mccycle.page_present = get_byte_bit(this->cpu.memory.memchip1[ptb_mem_addr], 3);
			this->cpu.microcode.mccycle.page_writable = get_byte_bit(this->cpu.memory.memchip1[ptb_mem_addr], 4);
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


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////






void SOL1_COMPUTER::mc_sequencer(long *runtime_counter) {


	char str_out[255];
	//this->hw_tty.print("#################################################################################\n");
	//sprintf(str_out,"# Runtime Counter: %d\n", (*runtime_counter)++); this->hw_tty.print(str_out)
	//this->hw_tty.print("#################################################################################\n");


	////////////////////////////////////////////////////////////////////////////
	this->cpu.mc_seq_update(this->hw_tty);
	refresh_int();

	SOL1_BYTE current_opcode = get_current_opcode();
	SOL1_BYTE current_opcode_cycle = get_current_opcode_cycle();


	if ((this->cpu.DEBUG_LOG_OPCODE) && (current_opcode > 0))
		//if ((get_current_opcode() != 0 || (get_current_opcode() == 0 && (get_current_opcode_cycle(this->cpu) < 10 | get_current_opcode_cycle(this->cpu) > 14))))
		disassembly_current_opcode();


	if (!(this->cpu.rom.bkpt_opcode == 0 && this->cpu.rom.bkpt_cycle == 0) &&
		(current_opcode == this->cpu.rom.bkpt_opcode && current_opcode_cycle == this->cpu.rom.bkpt_cycle))
	{

		sprintf(str_out, " Microcode Breakpoint | Starting Opcode/Cycle:%02x%02x.\n", this->cpu.rom.bkpt_opcode, this->cpu.rom.bkpt_cycle); this->hw_tty.print(str_out);
		this->cpu.DEBUG_MICROCODE = 1;
		this->cpu.DEBUG_REGISTERS = 1;
		this->cpu.DEBUG_ALU = 1;
		step = 1;
		if (this->cpu.DEBUG_MICROCODE) {
			this->hw_tty.print("***** MICROCODE\n");
			//sprintf(str_out, "U-ADDRESS: ");  print_word_bin(str_out + strlen(str_out), this->cpu.microcode.u_ad_bus); sprintf(str_out + strlen(str_out), "\n"); this->hw_tty.print(str_out);
			//sprintf(str_out, "OPCODE: %02x (cycle %02x)\n", current_opcode, current_opcode_cycle); this->hw_tty.print(str_out);
			//sprintf(str_out, "microcode: \n"); this->hw_tty.print(str_out);
			sprintf(str_out, "* U_FLAGS="); print_byte_bin(str_out + strlen(str_out), this->cpu.microcode.U_FLAGS.value()); sprintf(str_out + strlen(str_out), "\n"); this->hw_tty.print(str_out);
			this->cpu.rom.display_current_cycles_desc(current_opcode, current_opcode_cycle, this->hw_tty);
			this->hw_tty.print("\n");
		}
		debugmenu_main(this->cpu, this->hw_tty);
	}


	////////////////////////////////////////////////////////////////////////////
	// MEMORY READ

	//IC7  //IC24 //IC19 //183

	if ((this->cpu.microcode.mccycle.mdr_out_en & 0b00000001) == 0x01) {
		switch (this->cpu.microcode.mccycle.mdr_out_src & 0b00000001) {
		case 0x00:
			this->bus.data_bus = this->cpu.registers.MDRl.value();
			break;
		case 0x01:
			this->bus.data_bus = this->cpu.registers.MDRh.value();
			break;
		}
	}
	//244 MICROCODE SIGNALS NEED
	//INVERTER BECAUSE THEY GET RESET TO
	//ZERO AT BEGINNING, AND 244 ARE LOW ACTIVE.
	//THIS CAUSES PROBLEMS ON RESET.

	////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////


	if (buffer_rd() == 0x00) {
		unsigned long mem_addr = read_address_bus();


		if (mem_addr < 0xFF80 || mem_addr > 0xFFFF) {

			this->bus.data_bus = this->cpu.get_current_memory()[mem_addr];

			if (this->cpu.DEBUG_RDMEM && get_current_opcode() > 0) {
				char log[255];
				if (this->bus.data_bus == 0x00)
					sprintf(log, "         \t\t\t\tREAD  RAM [%04x]\t= %02x \'\\0\'\n", mem_addr, this->bus.data_bus);
				else if (this->bus.data_bus == 0x09)
					sprintf(log, "         \t\t\t\tREAD  RAM [%04x]\t= %02x \'\\t\'\n", mem_addr, this->bus.data_bus);
				else if (this->bus.data_bus == 0x0a)
					sprintf(log, "         \t\t\t\tREAD  RAM [%04x]\t= %02x \'\\r\'\n", mem_addr, this->bus.data_bus);
				else if (this->bus.data_bus == 0x0d)
					sprintf(log, "         \t\t\t\tREAD  RAM [%04x]\t= %02x \'\\n\'\n", mem_addr, this->bus.data_bus);
				else
					sprintf(log, "         \t\t\t\tREAD  RAM [%04x]\t= %02x \'%c\'\n", mem_addr, this->bus.data_bus, this->bus.data_bus);

				save_to_log(str_out, fa, log);
				//this->hw_tty.print(str_out);
			}
		}
		else if (mem_addr >= 0xFF80 && mem_addr <= 0xFFFF) {

			//RTC I/O bank = FFA0 to FFAF
			if (mem_addr >= 0xFFA0 && mem_addr <= 0xFFAF) {
				this->bus.data_bus = this->hw_rtc.data[mem_addr - 0xFFA0];

				if ((this->cpu.DEBUG_RTC) > 0) {

					char log_rtc[255];
					hw_rtc_print(&this->hw_rtc, (char*)"READ ", (mem_addr - 0xFFA0), log_rtc);

					save_to_log(str_out, fa, log_rtc);
					//this->hw_tty.print(str_out);

				}
			}

			//TIMER FFE0 - FFE3
			else if (mem_addr >= 0xFFE0 && mem_addr <= 0xFFE3) {
				this->bus.data_bus = this->hw_timer.data[mem_addr - 0xFFE0];

				if ((this->cpu.DEBUG_TIMER) > 0) {

					char log_timer[255];
					hw_timer_print(&this->hw_timer, (char*)"READ ", (mem_addr - 0xFFE0), log_timer);

					save_to_log(str_out, fa, log_timer);
					//this->hw_tty.print(str_out);
				}
			}

			//UART SERVICES INTERRUPT
			else if (mem_addr >= 0xFF80 && mem_addr <= 0xFF85) {

				hw_uart_get_lsr(&this->hw_uart);

				if (mem_addr - 0xFF80 == 0) {
					hw_uart_read(&this->hw_uart);
				}

				this->bus.data_bus = this->hw_uart.data[mem_addr - 0xFF80];

				if ((this->cpu.DEBUG_UART) && (get_current_opcode() > 0)) {
					char log_uart[255];
					hw_uart_print(&this->hw_uart, (char*)"READ", (mem_addr - 0xFF80), log_uart);

					save_to_log(str_out, fa, log_uart);
					//this->hw_tty.print(str_out);

				}

			}


			if (mem_addr >= 0xFFD0 && mem_addr <= 0xFFD7) {


				if (mem_addr - 0xFFD0 == 0)
					hw_ide_read(&this->hw_ide);

				this->bus.data_bus = this->hw_ide.data[mem_addr - 0xFFD0];


				if ((this->cpu.DEBUG_IDE & get_current_opcode()) > 0) {
					char log_ide[255];
					hw_ide_print(&this->hw_ide, (char*)"READ ", (mem_addr - 0xFFD0), log_ide);

					save_to_log(str_out, fa, log_ide);
					//this->hw_tty.print(str_out);

				}

			}

		}
	}

	////////////////////////////////////////////////////////////////////////////
	// W = X
	this->bus.w_bus = this->bus.w_bus_refresh(this->cpu.registers, this->cpu.microcode.mccycle.panel_regsel,
		this->cpu.alu.alu_a_src, this->cpu.microcode.mccycle.display_reg_load && this->cpu.display_reg_load,
		this->cpu.microcode.mccycle.int_vector, this->cpu.registers.INT_MASKS.value(), this->cpu.microcode.mccycle.int_status, fa, this->cpu.DEBUG_RDREG, this->hw_tty);

	////////////////////////////////////////////////////////////////////////////
	// K = Y
	this->bus.k_bus = this->bus.k_bus_refresh(this->cpu.registers, this->cpu.alu.alu_b_src);
	////////////////////////////////////////////////////////////////////////////
	//IC184 //IC204 //IC170 //IC179 //IC181 //IC182
	this->bus.x_bus = this->bus.x_bus_refresh(this->cpu.registers, this->cpu.alu.alu_a_src, this->bus.w_bus);

	////////////////////////////////////////////////////////////////////////////
	//IC98 //IC22
	this->bus.y_bus = (this->cpu.alu.alu_b_src == 0x00) ? this->cpu.microcode.mccycle.imm : this->bus.k_bus;


	///////////////////////////////////////////////////////////////////////////
	//SOL1_BYTE clk = 0x1;
	//SOL1_BYTE rst = 0x1;
	//clk //IC28
	if (this->cpu.microcode.mccycle.ir_wrt == 0x00) // SETA IR ANTES DA OPERACAO
		this->cpu.microcode.IR.set(this->bus.data_bus);
	////////////////////////////////////////////////////////////////////////////
	// ALU

	//sol1_u_flags(this->cpu, alu, bus->z_bus);
	//update_final_condition(this->cpu);

	this->cpu.refresh_MSWh_ZF(this->bus.z_bus, this->cpu.microcode.mccycle.zf_in_src);
	this->cpu.refresh_MSWh_CF(this->bus.z_bus, this->cpu.microcode.mccycle.cf_in_src);
	this->cpu.refresh_MSWh_SF(this->bus.z_bus, this->cpu.microcode.mccycle.sf_in_src);
	this->cpu.refresh_MSWh_OF(this->bus.z_bus, this->cpu.microcode.u_sf, this->cpu.microcode.mccycle.of_in_src);

	this->cpu.microcode.u_flags(this->cpu.alu, this->bus.z_bus, this->cpu.DEBUG_UFLAGS, this->hw_tty);
	this->cpu.microcode.update_final_condition(this->cpu.registers);



	SOL1_BYTE alu_result = ALU_EXEC(&this->cpu.alu, this->bus.x_bus, this->bus.y_bus,
		this->cpu.microcode.u_cf,
		get_byte_bit(this->cpu.registers.MSWh.value(), MSWh_CF),
		this->cpu.microcode.mccycle.shift_src,
		this->cpu.microcode.mccycle.zbus_out_src, this->cpu.DEBUG_ALU, this->hw_tty);

	////////////////////////////////////////////////////////////////////////////


	this->bus.z_bus = alu_result;


	//#######################

	if (buffer_wr() == 0x00) {
		unsigned long mem_addr = read_address_bus();


		if (mem_addr < 0xFF80 || mem_addr > 0xFFFF) {

			if (this->cpu.DEBUG_WRMEM && get_current_opcode() > 0)
			{
				char log[255];
				if (this->bus.data_bus == 0x00)
					sprintf(log, "         \t\t\t\tWRITE RAM [%04x]\t= %02x \'\\0'\n", mem_addr, this->bus.data_bus);
				else if (this->bus.data_bus == 0x09)
					sprintf(log, "         \t\t\t\tWRITE RAM [%04x]\t= %02x \'\\t'\n", mem_addr, this->bus.data_bus);
				else if (this->bus.data_bus == 0x0a)
					sprintf(log, "         \t\t\t\tWRITE RAM [%04x]\t= %02x \'\\r\'\n", mem_addr, this->bus.data_bus);
				else if (this->bus.data_bus == 0x0d)
					sprintf(log, "         \t\t\t\tWRITE RAM [%04x]\t= %02x \'\\n\'\n", mem_addr, this->bus.data_bus);
				else
					sprintf(log, "         \t\t\t\tWRITE RAM [%04x]\t= %02x \'%c\'\n", mem_addr, this->bus.data_bus, this->bus.data_bus);

				save_to_log(str_out, fa, log);
				//this->hw_tty.print(str_out);
			}

			this->cpu.get_current_memory()[mem_addr] = this->bus.data_bus;

		}
		else if (mem_addr >= 0xFF80 && mem_addr <= 0xFFFF) {

			//RTC I/O bank = FFA0 to FFAF
			if (mem_addr >= 0xFFA0 && mem_addr <= 0xFFAF) {
				this->hw_rtc.data[mem_addr - 0xFFA0] = this->bus.data_bus;


				if ((this->cpu.DEBUG_RTC) > 0) {
					char log_rtc[255];
					hw_rtc_print(&this->hw_rtc, (char*)"WRITE", (mem_addr - 0xFFA0), log_rtc);

					save_to_log(str_out, fa, log_rtc);
					//this->hw_tty.print(str_out);
				}
			}

			//TIMER FFE0 - FFE3
			else if (mem_addr >= 0xFFE0 && mem_addr <= 0xFFE3) {

				if (mem_addr - 0xFFE0 == 0x00)
					hw_timer_set_c0(&this->hw_timer, this->bus.data_bus);

				else if (mem_addr - 0xFFE0 == 0x01)
					hw_timer_set_c1(&this->hw_timer, this->bus.data_bus);

				else
					this->hw_timer.data[mem_addr - 0xFFE0] = this->bus.data_bus;

				if ((this->cpu.DEBUG_TIMER) > 0) {

					char log_timer[255];
					hw_timer_print(&this->hw_timer, (char*)"WRITE", (mem_addr - 0xFFE0), log_timer);

					save_to_log(str_out, fa, log_timer);
					//this->hw_tty.print(str_out);
				}
			}

			//UART SERVICES INTERRUPT
			else if (mem_addr >= 0xFF80 && mem_addr <= 0xFF85) {

				hw_uart_get_lsr(&this->hw_uart);

				if (mem_addr - 0xFF80 == 0 && (this->hw_uart.data[5] == 0x20 || this->hw_uart.data[5] == 0x01)) {
					hw_uart_send(&this->hw_uart, this->bus.data_bus);


					this->hw_tty.send(this->bus.data_bus);


					if (this->cpu.WEB_SERVER)
						this->hw_web.new_char(this->bus.data_bus);

				}
				else
					this->hw_uart.data[mem_addr - 0xFF80] = this->bus.data_bus;

				if ((this->cpu.DEBUG_UART & get_current_opcode()) > 0) {
					//hw_uart_write(this->cpu, uart_out, this->bus.data_bus);
					char log_uart[255];
					hw_uart_print(&this->hw_uart, (char*)"WRITE", (mem_addr - 0xFF80), log_uart);

					save_to_log(str_out, fa, log_uart);
					//this->hw_tty.print(str_out);
				}

			}

			if (mem_addr >= 0xFFD0 && mem_addr <= 0xFFD7) {

				this->hw_ide.data[mem_addr - 0xFFD0] = this->bus.data_bus;

				if ((this->cpu.DEBUG_IDE & get_current_opcode()) > 0) {
					char log_ide[255];
					hw_ide_print(&this->hw_ide, (char*)"WRITE", (mem_addr - 0xFFD0), log_ide);

					save_to_log(str_out, fa, log_ide);
					//this->hw_tty.print(str_out);
				}

				if (mem_addr - 0xFFD0 == 0) {
					hw_ide_write(&this->hw_ide);
				}

				// SET HD NEW STATUS AFTER LOG
				if (this->hw_ide.data[7] == 0x04) { // RESET IDE
					this->hw_ide.data[7] = 0x0; // 0x80 ==busy// is ready again
				}

				else if (this->hw_ide.data[7] == 0xEF) { // SET FEATURE COMMAND
					this->hw_ide.data[7] = 0x00; // is ready again
				}

				else if (this->hw_ide.data[7] == 0xE6) { // SLEEP
					this->hw_ide.data[7] = 0x00;// zerar 
					//hw_ide.data[7] = 0x80; // is ready again
				}

				else if (this->hw_ide.data[7] == 0x20) { // read sector cmd
					this->hw_ide.data[7] = 0b00001000;
					hw_ide_reset(&this->hw_ide);
				}
				else if (this->hw_ide.data[7] == 0x30) { // write sector cmd
					this->hw_ide.data[7] = 0b00001000;
					hw_ide_reset(&this->hw_ide);
				}
			}
		}

	}


	//}
	//#######################
	//IC86B //IC58B //IC86C //IC241 //IC14 //IC255 //IC23

	//
	SOL1_BYTE inMSWh_ZF = this->cpu.refresh_MSWh_ZF(this->bus.z_bus, this->cpu.microcode.mccycle.zf_in_src);
	SOL1_BYTE inMSWh_CF = this->cpu.refresh_MSWh_CF(this->bus.z_bus, this->cpu.microcode.mccycle.cf_in_src);
	SOL1_BYTE inMSWh_SF = this->cpu.refresh_MSWh_SF(this->bus.z_bus, this->cpu.microcode.mccycle.sf_in_src);
	SOL1_BYTE inMSWh_OF = this->cpu.refresh_MSWh_OF(this->bus.z_bus, this->cpu.microcode.u_sf, this->cpu.microcode.mccycle.of_in_src);

	//

	if (0x01) { // ~RST
		//IC206
		SOL1_BYTE inMSW_H = set_byte_bit(inMSWh_ZF, 0) | set_byte_bit(inMSWh_CF, 1) | set_byte_bit(inMSWh_SF, 2) | set_byte_bit(inMSWh_OF, 3);
		this->cpu.registers.MSWh.set(inMSW_H);
	}

	if (this->cpu.microcode.mccycle.status_wrt == 0x00) {
		//SOL1_BYTE oldStatus = this->cpu.registers.MSWl.value();
		this->cpu.registers.MSWl.set(this->bus.z_bus);
	}



	///////////////////////////////////////////////////////////////////////////
	// READ DATA
	//DATA REGISTERS
	if (this->cpu.microcode.mccycle.ah_wrt == 0x00) { this->cpu.registers.Ah.set(this->bus.z_bus); if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"Ah", this->bus.z_bus); } }
	if (this->cpu.microcode.mccycle.al_wrt == 0x00) { this->cpu.registers.Al.set(this->bus.z_bus); if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"Al", this->bus.z_bus); } }

	if (this->cpu.microcode.mccycle.bh_wrt == 0x00) { this->cpu.registers.Bh.set(this->bus.z_bus); if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"Bh", this->bus.z_bus); } }
	if (this->cpu.microcode.mccycle.bl_wrt == 0x00) { this->cpu.registers.Bl.set(this->bus.z_bus); if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"Bl", this->bus.z_bus); } }

	if (this->cpu.microcode.mccycle.ch_wrt == 0x00) { this->cpu.registers.Ch.set(this->bus.z_bus); if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"Ch", this->bus.z_bus); } }
	if (this->cpu.microcode.mccycle.cl_wrt == 0x00) { this->cpu.registers.Cl.set(this->bus.z_bus); if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"Cl", this->bus.z_bus); } }

	if (this->cpu.microcode.mccycle.dh_wrt == 0x00) { this->cpu.registers.Dh.set(this->bus.z_bus); if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"Dh", this->bus.z_bus); } }
	if (this->cpu.microcode.mccycle.dl_wrt == 0x00) { this->cpu.registers.Dl.set(this->bus.z_bus); if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"Dl", this->bus.z_bus); } }

	if (this->cpu.microcode.mccycle.gh_wrt == 0x00) { this->cpu.registers.Gh.set(this->bus.z_bus); if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"Gh", this->bus.z_bus); } }
	if (this->cpu.microcode.mccycle.gl_wrt == 0x00) { this->cpu.registers.Gl.set(this->bus.z_bus); if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"Gl", this->bus.z_bus); } }

	//Pointer Registers
	if (this->cpu.microcode.mccycle.bph_wrt == 0x00) { this->cpu.registers.BPh.set(this->bus.z_bus); if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"BPh", this->bus.z_bus); } }
	if (this->cpu.microcode.mccycle.bpl_wrt == 0x00) { this->cpu.registers.BPl.set(this->bus.z_bus); if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"BPl", this->bus.z_bus); } }


	/*
	if (!check_byte_bit(this->cpu.registers.MSWl.value(), MSWl_CPU_MODE)) {

		if ((this->cpu.microcode.mccycle.sph_wrt == 0x00) || (this->cpu.microcode.mccycle.ssph_wrt == 0x00)) {
			this->cpu.registers.SPh.set(this->bus.z_bus); if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"SPh", this->bus.z_bus); }
			this->cpu.registers.SSPh.set(this->bus.z_bus); if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"SSPh", this->bus.z_bus); }
		}
		if ((this->cpu.microcode.mccycle.spl_wrt == 0x00) || this->cpu.microcode.mccycle.sspl_wrt == 0x00) {
			this->cpu.registers.SPl.set(this->bus.z_bus); if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"SPl", this->bus.z_bus); }
			this->cpu.registers.SSPl.set(this->bus.z_bus); if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"SSPl", this->bus.z_bus); }
		}
	}
	else {
		if (this->cpu.microcode.mccycle.sph_wrt == 0x00) { this->cpu.registers.SPh.set(this->bus.z_bus); if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"SPh", this->bus.z_bus); } }
		if (this->cpu.microcode.mccycle.spl_wrt == 0x00) { this->cpu.registers.SPl.set(this->bus.z_bus); if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"SPl", this->bus.z_bus); } }


		if (this->cpu.microcode.mccycle.ssph_wrt == 0x00) { this->cpu.registers.SSPh.set(this->bus.z_bus); if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"SSPh", this->bus.z_bus); } }
		if (this->cpu.microcode.mccycle.sspl_wrt == 0x00) { this->cpu.registers.SSPl.set(this->bus.z_bus); if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"SSPl", this->bus.z_bus); } }
	}
	*/

	if (!check_byte_bit(this->cpu.registers.MSWl.value(), MSWl_CPU_MODE)) {

		if ((this->cpu.microcode.mccycle.sph_wrt == 0x00) || (this->cpu.microcode.mccycle.ssph_wrt == 0x00)) {
			//this->cpu.registers.SPh.set(this->bus.z_bus); if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"SPh", this->bus.z_bus); }
			this->cpu.registers.SSPh.set(this->bus.z_bus); if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"SSPh", this->bus.z_bus); }
		}
		if ((this->cpu.microcode.mccycle.spl_wrt == 0x00) || this->cpu.microcode.mccycle.sspl_wrt == 0x00) {
			//this->cpu.registers.SPl.set(this->bus.z_bus); if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"SPl", this->bus.z_bus); }
			this->cpu.registers.SSPl.set(this->bus.z_bus); if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"SSPl", this->bus.z_bus); }
		}
	}
	if (this->cpu.microcode.mccycle.sph_wrt == 0x00) { this->cpu.registers.SPh.set(this->bus.z_bus); if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"SPh", this->bus.z_bus); } }
	if (this->cpu.microcode.mccycle.spl_wrt == 0x00) { this->cpu.registers.SPl.set(this->bus.z_bus); if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"SPl", this->bus.z_bus); } }


	//Index Registers
	if (this->cpu.microcode.mccycle.sih_wrt == 0x00) { this->cpu.registers.SIh.set(this->bus.z_bus); if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"SIh", this->bus.z_bus); } }
	if (this->cpu.microcode.mccycle.sil_wrt == 0x00) { this->cpu.registers.SIl.set(this->bus.z_bus); if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"SIl", this->bus.z_bus); } }

	if (this->cpu.microcode.mccycle.dih_wrt == 0x00) { this->cpu.registers.DIh.set(this->bus.z_bus); if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"DIh", this->bus.z_bus); } }
	if (this->cpu.microcode.mccycle.dil_wrt == 0x00) { this->cpu.registers.DIl.set(this->bus.z_bus); if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"DIl", this->bus.z_bus); } }

	if (this->cpu.microcode.mccycle.pch_wrt == 0x00) { this->cpu.registers.PCh.set(this->bus.z_bus); if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"PCh", this->bus.z_bus); } }
	if (this->cpu.microcode.mccycle.pcl_wrt == 0x00) { this->cpu.registers.PCl.set(this->bus.z_bus); if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"PCl", this->bus.z_bus); } }

	if (this->cpu.microcode.mccycle.tdrh_wrt == 0x00) { this->cpu.registers.TDRh.set(this->bus.z_bus); if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"TDRh", this->bus.z_bus); } }
	if (this->cpu.microcode.mccycle.tdrl_wrt == 0x00) { this->cpu.registers.TDRl.set(this->bus.z_bus); if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"TDRl", this->bus.z_bus); } }

	if (this->cpu.microcode.mccycle.ptb_wrt == 0x00) { this->cpu.registers.PTB.set(this->bus.z_bus); if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"PTB", this->bus.z_bus); } }

	/////////////////////////////////////////////////////////////////////////////
	if (this->cpu.microcode.mccycle.mask_flags_wrt == 0x00) this->cpu.registers.INT_MASKS.set(this->bus.z_bus);
	/////////////////////////////////////////////////////////////////////////////
	// SET MDR
	 //IC7 //IC24 //IC19 //IC183

	if (this->cpu.microcode.mccycle.mdrl_wrt == 0x00) { this->cpu.registers.MDRl.set(this->cpu.microcode.mccycle.mdr_in_src == 0x00 ? this->bus.z_bus : this->bus.data_bus);  if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"MDRl", (this->cpu.microcode.mccycle.mdr_in_src == 0x00 ? this->bus.z_bus : this->bus.data_bus)); } }
	if (this->cpu.microcode.mccycle.mdrh_wrt == 0x00) { this->cpu.registers.MDRh.set(this->cpu.microcode.mccycle.mdr_in_src == 0x00 ? this->bus.z_bus : this->bus.data_bus); if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"MDRh", (this->cpu.microcode.mccycle.mdr_in_src == 0x00 ? this->bus.z_bus : this->bus.data_bus)); } }
	////////////////////////////////////////////////////////////////////////////
	//MEMORY SET MAR

	if (this->cpu.microcode.mccycle.marl_wrt == 0x00) {
		//IC131 //IC128			
		if (this->cpu.microcode.mccycle.mar_in_src == 0x00)
		{
			this->cpu.registers.MARl.set(this->bus.z_bus);
			if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"MARl", this->bus.z_bus); }
		}
		else {
			this->cpu.registers.MARl.set(this->cpu.registers.PCl.value());
			if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"MARl", this->cpu.registers.PCl.value()); }
		}
	}

	if (this->cpu.microcode.mccycle.marh_wrt == 0x00) {
		//IC129 //IC132			
		if (this->cpu.microcode.mccycle.mar_in_src == 0x00) {
			this->cpu.registers.MARh.set(this->bus.z_bus);
			if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"MARh", this->bus.z_bus); }
		}
		else {
			this->cpu.registers.MARh.set(this->cpu.registers.PCh.value());
			if (this->cpu.DEBUG_WRREG) { reg8bit_print(fa, (char*)"WRITE", (char*)"MARh", this->cpu.registers.PCh.value()); }
		}
	}

	if (this->cpu.microcode.mccycle.pagtbl_ram_we == 0x01)
	{
		SOL1_MWORD ptb_mem_addr = ((this->cpu.registers.MARh.value() >> 3) & 0b00011111);
		if (this->cpu.microcode.page_table_addr_src(this->cpu.registers) == 0x01)
			ptb_mem_addr = ptb_mem_addr | this->cpu.registers.PTB.value() << 5;

		if (this->cpu.microcode.mccycle.mdr_to_pagtbl_en == 0x1) {
			this->cpu.memory.memchip0[ptb_mem_addr] = this->cpu.registers.MDRl.value();
			this->cpu.memory.memchip1[ptb_mem_addr] = this->cpu.registers.MDRh.value();

		}

		else {
			this->cpu.memory.memchip0[ptb_mem_addr] = 0;
			this->cpu.memory.memchip1[ptb_mem_addr] = 0;
		}
	}




	////////////////////////////////////////////////////////////////////////////

	if (this->cpu.DEBUG_BUSES) {
		this->hw_tty.print("***** BUS\n");
		sprintf(str_out, "* u_ad_bus    : "); print_word_bin(str_out + strlen(str_out), this->cpu.microcode.u_ad_bus); sprintf(str_out + strlen(str_out), "\n"); this->hw_tty.print(str_out);
		sprintf(str_out, "* address bus : "); print_byte_bin(str_out + strlen(str_out), (SOL1_BYTE)(read_address_bus() >> 16)); sprintf(str_out + strlen(str_out), " ");  print_word_bin(str_out + strlen(str_out), read_address_bus()); sprintf(str_out + strlen(str_out), "\n"); this->hw_tty.print(str_out);
		sprintf(str_out, "*  data_bus   |"); this->hw_tty.print(str_out);
		sprintf(str_out, "    k_bus    |"); this->hw_tty.print(str_out);
		sprintf(str_out, "    w_bus    |"); this->hw_tty.print(str_out);
		sprintf(str_out, "    x_bus    |"); this->hw_tty.print(str_out);
		sprintf(str_out, "    y_bus    |"); this->hw_tty.print(str_out);
		sprintf(str_out, "    z_bus    "); this->hw_tty.print(str_out);
		this->hw_tty.print("\n");
		sprintf(str_out, "  "); print_byte_bin(str_out + strlen(str_out), this->bus.data_bus); this->hw_tty.print(str_out);
		sprintf(str_out, " | "); print_byte_bin(str_out + strlen(str_out), this->bus.k_bus); this->hw_tty.print(str_out);
		sprintf(str_out, " | "); print_byte_bin(str_out + strlen(str_out), this->bus.w_bus); this->hw_tty.print(str_out);
		sprintf(str_out, " | "); print_byte_bin(str_out + strlen(str_out), this->bus.x_bus); this->hw_tty.print(str_out);
		sprintf(str_out, " | "); print_byte_bin(str_out + strlen(str_out), this->bus.y_bus); this->hw_tty.print(str_out);
		sprintf(str_out, " | "); print_byte_bin(str_out + strlen(str_out), this->bus.z_bus); this->hw_tty.print(str_out);
		this->hw_tty.print("\n");
		this->hw_tty.print("\n");
	}

	if (this->cpu.DEBUG_MEMORY) {
		//this->hw_tty.print("***** MEMORY\n"); 
		this->cpu.memory.display(this->cpu.registers, this->hw_tty);
	}

	if (this->cpu.DEBUG_REGISTERS) {
		this->hw_tty.print("***** REGISTERS\n");
		this->cpu.display_registers(this->hw_tty);

	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//CLOCK HIGH
	////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	this->cpu.refresh_MSWh_ZF(this->bus.z_bus, this->cpu.microcode.mccycle.zf_in_src);
	this->cpu.refresh_MSWh_CF(this->bus.z_bus, this->cpu.microcode.mccycle.cf_in_src);
	this->cpu.refresh_MSWh_SF(this->bus.z_bus, this->cpu.microcode.mccycle.sf_in_src);
	this->cpu.refresh_MSWh_OF(this->bus.z_bus, this->cpu.microcode.u_sf, this->cpu.microcode.mccycle.of_in_src);

	this->cpu.microcode.u_flags(this->cpu.alu, this->bus.z_bus, this->cpu.DEBUG_UFLAGS, this->hw_tty);
	this->cpu.microcode.update_final_condition(this->cpu.registers);

	this->cpu.microcode.u_adder_refresh(this->cpu.microcode.mccycle.next, this->cpu.microcode.mccycle.final_condition);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if (this->cpu.DEBUG_UADDER) {
		this->hw_tty.print("***** U_ADDER\n");
		this->cpu.microcode.display_u_adder(this->cpu.microcode.mccycle.next, this->cpu.microcode.mccycle.final_condition, this->hw_tty);
		this->hw_tty.print("\n");
	}

	if (!(this->cpu.rom.bkpt_opcode == 0 && this->cpu.rom.bkpt_cycle == 0) &&
		(current_opcode == this->cpu.rom.bkpt_opcode &&
			current_opcode_cycle == this->cpu.rom.bkpt_cycle))
	{
		sprintf(str_out, " Microcode Breakpoint | Opcode/Cycle:%02x%02x Finished.\n", this->cpu.rom.bkpt_opcode, this->cpu.rom.bkpt_cycle); this->hw_tty.print(str_out);
		debugmenu_main(this->cpu, this->hw_tty);
	}

	if ((this->cpu.BKPT != 0xFFFF) && (SOL1_REGISTERS::value(this->cpu.registers.MARl, this->cpu.registers.MARh) == this->cpu.BKPT
		|| SOL1_REGISTERS::value(this->cpu.registers.PCl, this->cpu.registers.PCh) == this->cpu.BKPT))
	{
		this->cpu.DEBUG_MICROCODE = 1;
		this->cpu.DEBUG_REGISTERS = 1;
		this->cpu.DEBUG_ALU = 1;
		step = 1;
		if (this->cpu.DEBUG_MICROCODE) {
			this->hw_tty.print("\n***** MICROCODE\n");
			//sprintf(str_out, "U-ADDRESS: ");  print_word_bin(str_out + strlen(str_out), this->cpu.microcode.u_ad_bus); sprintf(str_out + strlen(str_out), "\n"); this->hw_tty.print(str_out);
			//sprintf(str_out, "OPCODE: %02x (cycle %02x)\n", current_opcode, current_opcode_cycle); this->hw_tty.print(str_out);
			//sprintf(str_out, "microcode: \n"); this->hw_tty.print(str_out);
			sprintf(str_out, "* U_FLAGS="); print_byte_bin(str_out + strlen(str_out), this->cpu.microcode.U_FLAGS.value()); sprintf(str_out + strlen(str_out), "\n"); this->hw_tty.print(str_out);
			this->cpu.rom.display_current_cycles_desc(current_opcode, current_opcode_cycle, this->hw_tty);
			this->hw_tty.print("\n");
		}

		sprintf(str_out, " Memory Breakpoint | Starting Address:%04x.\n", this->cpu.BKPT); this->hw_tty.print(str_out);
		debugmenu_main(this->cpu, this->hw_tty);
	}
	/*
	else if ((this->cpu.BKPT != 0xFFFF) && (sol1_registers_value(this->cpu.registers.MARl, this->cpu.registers.MARh) == this->cpu.BKPT
		|| sol1_registers_value(this->cpu.registers.PCl, this->cpu.registers.PCh) == this->cpu.BKPT))
	{
		sprintf(str_out, " Memory Breakpoint | Address:%04x Finished.\n", this->cpu.BKPT); this->hw_tty.print(str_out);
		debugmenu_main(this->cpu);
	}
	*/


	hw_timer_tick_c0(&this->hw_timer);

	this->bus.reset();
}


void SOL1_COMPUTER::RunCPU(long *runtime_counter) {

#if defined(__linux__) || defined(__MINGW32__)
	struct timespec tstart = { 0,0 }, tend = { 0,0 };

#elif _MSC_VER    
	auto tstart = high_resolution_clock::now();
	auto tend = high_resolution_clock::now();
#endif 

	char str_out[255];

	double deltaTime = 0;
	double cpu_clk = 0;
	
	long ticks = 0;
	while (1) {

#if defined(__linux__) || defined(__MINGW32__)
		clock_gettime(CLOCK_MONOTONIC, &tend);
		deltaTime = ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec);
		clock_gettime(CLOCK_MONOTONIC, &tstart);

#elif _MSC_VER        
		tend = high_resolution_clock::now();
		deltaTime = duration_cast<nanoseconds>(tend - tstart).count()*1e-9;
		tstart = high_resolution_clock::now();
#endif 
		cpu_clk += deltaTime;

		mc_sequencer(runtime_counter);

		SOL1_BYTE current_opcode = get_current_opcode();
		SOL1_BYTE current_opcode_cycle = get_current_opcode_cycle();

		if (this->cpu.microcode.MUX(this->cpu.registers) == 0x02) {
			if (!this->cpu.SERVER) {

				if (kbhit()) {
					char dddd = this->hw_tty.get_char();


					if (dddd == 0x04) {
						this->hw_tty.debug_call = 1;
					}
					else {
						hw_uart_receive(&this->hw_uart, dddd);
						//this->cpu.microcode.mccycle.int_request = 0x01;
					}
				}
			}
		}


		if (this->hw_tty.debug_call == 1) {
			this->hw_tty.debug_call = 0;
			this->hw_tty.send('\r');
			this->hw_tty.send('\n');
			return;
		}

		if (this->hw_uart.data[5] == 0xFF || this->hw_uart.data[5] == 0x20)
			if (!queue_empty(this->hw_uart.uart_in))
				hw_uart_write(&this->hw_uart);





		if ((this->cpu.DEBUG_LITE && (current_opcode != oldOP || SOL1_REGISTERS::value(this->cpu.registers.PCl, this->cpu.registers.PCh) != oldPC)) || this->cpu.DEBUG_LITE_CYCLES) {

			SOL1_BYTE cur_opcode = current_opcode;
			SOL1_BYTE cur_cycle = current_opcode_cycle;
			SOL1_BYTE pc_opcode = this->cpu.get_current_memory()[SOL1_REGISTERS::value(this->cpu.registers.PCl, this->cpu.registers.PCh)];

			if (!(cur_opcode == 0x00 && cur_cycle == 0x10)) {
				char str[100];
				char log_reg_lite[256];

				char temp[4];


				sprintf(temp, "%02x", current_opcode);

				SOL1_MWORD memADDR = SOL1_REGISTERS::value(this->cpu.registers.PCl, this->cpu.registers.PCh);

				sprintf(str, " Opcode=%02x:%s", cur_opcode, &this->cpu.rom.rom_desc[0x400000 + (cur_opcode * 256)]);

				sprintf(log_reg_lite, "%04x] %s", (int)(memADDR - (strlen(temp) / 2)), rightpad(str, 40)); //this->hw_tty.print(str_out);

				sprintf(log_reg_lite + strlen(log_reg_lite), " | Cycle: %02x", cur_cycle); //this->hw_tty.print(str_out);
				sprintf(log_reg_lite + strlen(log_reg_lite), " | PC: %04x", SOL1_REGISTERS::value(this->cpu.registers.PCl, this->cpu.registers.PCh)); //this->hw_tty.print(str_out);

				/*
				sprintf(log_reg_lite + strlen(log_reg_lite), " | @PC=%02x:%s\n", pc_opcode, &this->cpu.rom.rom_desc[0x400000 + (pc_opcode * 256)]);//this->hw_tty.print(str_out);
				*/
				sprintf(log_reg_lite + strlen(log_reg_lite), " | MEM: %02x%02x%02x%02x\n",
					this->cpu.get_current_memory()[SOL1_REGISTERS::value(this->cpu.registers.MARl, this->cpu.registers.MARh)],
					this->cpu.get_current_memory()[SOL1_REGISTERS::value(this->cpu.registers.MARl, this->cpu.registers.MARh) + 1],
					this->cpu.get_current_memory()[SOL1_REGISTERS::value(this->cpu.registers.MARl, this->cpu.registers.MARh) + 2],
					this->cpu.get_current_memory()[SOL1_REGISTERS::value(this->cpu.registers.MARl, this->cpu.registers.MARh) + 3]
				); //this->hw_tty.print(str_out);

				save_to_log(str_out, fa, log_reg_lite);
				//this->hw_tty.print("\n");

			}
			oldOP = cur_opcode;
			oldPC = SOL1_REGISTERS::value(this->cpu.registers.PCl, this->cpu.registers.PCh);
		}

		if (step == 1 && this->cpu.microcode.MUX(this->cpu.registers) == 0x02) {
			this->hw_tty.print("###########################################\n");
			sprintf(str_out, "## End OpStep on Opcode/Cycle:%02x%02x. #######\n", current_opcode, current_opcode_cycle); this->hw_tty.print(str_out);
			this->hw_tty.print("###########################################\n");
			return;
		}
		else if (microcodestep == 1) {
			this->hw_tty.print("###########################################\n");
			sprintf(str_out, "## End MicroStep on Opcode/Cycle:%02x%02x. ####\n", current_opcode, current_opcode_cycle); this->hw_tty.print(str_out);
			this->hw_tty.print("###########################################\n");
			return;
		}

		if (microcodestep == 0 && step == 0) {

			ticks++;

			if (cpu_clk > 1) {
				//printf("%d\n", ticks);
				ticks = 0;
				cpu_clk = 0;
			}


			if (ticks > 3800000) { // limiting 3.8 mhz
#ifdef _MSC_VER     

				nanosleep(1);
#else
				struct timespec ts;
				ts.tv_sec = 0;
				ts.tv_nsec = 1;
				nanosleep(&ts, NULL);

#endif

			}
		}

	}

	this->cpu.microcode.mccycle.next = 0x00;
}






//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////



int SOL1_COMPUTER::init() {

	char str_out[255];
	char buff[255];
	//	char buff2[255];
	FILE *fp;

	fp = fopen("tasm1.tab", "r");

	if (fp == NULL)
	{
		this->hw_tty.print("Could not found tasm1.tab file!");
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
			this->ht_opcodes[strlower(_opcode)] = Tasm_Opcode(_opcode, _desc, _size);
		}
	}

	fclose(fp);

	this->cpu.init(this->hw_tty);

	fa = fopen("debug_trace.log", "w");
	if (fa == NULL) {
		this->hw_tty.print("can not open debug file\n");
		exit(1);
	}

	hw_uart_init(&this->hw_uart, &this->cpu);
	hw_rtc_init(&this->hw_rtc);
	hw_rtc_start_clock(&this->hw_rtc);
	hw_timer_init(&this->hw_timer);
	hw_ide_init(&this->hw_ide);
	//this->hw_tty.init(this->cpu, &this->hw_uart);

	this->cpu.memory.load_bios();

	hw_ide_load_disk(this->hw_ide.memory);

	//init bus
	this->bus.reset();

	if (this->cpu.SERVER) {
		this->hw_tty.start_server(&this->hw_uart);
	}

	if (this->cpu.WEB_SERVER) {
		this->hw_web.start_server(this->cpu, &this->hw_uart);
	}


	return 1;
}







void SOL1_COMPUTER::trace_menu() {
	this->hw_tty.print("SOL-1 Debug Monitor > Trace\n");
	this->hw_tty.print("\n");
	this->hw_tty.print("  S - Opcode Step\n");
	this->hw_tty.print("  M - Microcode Step\n");
	this->hw_tty.print("\n");
	this->hw_tty.print("  I - Back one microcode step\n");
	this->hw_tty.print("\n");
	this->hw_tty.print("  R - Display Registers\n");
	this->hw_tty.print("\n");
	this->hw_tty.print("  O - Display Memory\n");
	this->hw_tty.print("  P - Reset Memory\n");
	this->hw_tty.print("\n");
	this->hw_tty.print("  G - Go(Run)\n");
	this->hw_tty.print("\n");
	this->hw_tty.print("  ? - Display Menu\n");
	this->hw_tty.print("  Q - Back to Debug Monitor\n");
	this->hw_tty.print("\n");
	this->hw_tty.print("  ... or a sequence of opcodes to load into memory");
	this->hw_tty.print("\n");
}

void SOL1_COMPUTER::run() {

	char str_out[255];

	long runtime_counter = 0;

	//cpu_print(&z80);
	int run = 0;
	int debug = 0;

	int start = 1;
	while (1) {
		char *input = (char*)malloc(sizeof(char) * 257);

		//sol1_cpu_memory_display(&sol1_cpu);
		//sol1_cpu_display_registers_lite(&sol1_cpu);
		//this->hw_tty.print("Flags: "); mswh_flags_desc(&sol1_cpu); this->hw_tty.print("\n");
		//this->hw_tty.print("Status: "); mswl_status_desc(&sol1_cpu); this->hw_tty.print("\n");

		if (start == 1) {

			input[0] = 'q';
			input[1] = '\n';
			start = 0;
		}
		else {
			this->hw_tty.print("TRACE> ");
			input = this->hw_tty.getline();
		}

		if (input[0] != 'q' && input[0] != 'Q' &&
			input[0] != 'm' && input[0] != 'M' &&
			input[0] != 'r' && input[0] != 'R' &&
			input[0] != 'p' && input[0] != 'P' &&
			input[0] != 's' && input[0] != 'S' &&
			input[0] != 'o' && input[0] != 'O' &&
			input[0] != 'i' && input[0] != 'I' &&
			input[0] != 'g' && input[0] != 'G' &&
			input[0] != '?') {
			size_t numdigits = strlen(input) / 2;
			size_t i;

			int begin = SOL1_REGISTERS::value(this->cpu.registers.PCl, this->cpu.registers.PCh);
			for (i = 0; i != numdigits; ++i)
			{
				unsigned char output = 16 * toInt(input[2 * i]) + toInt(input[2 * i + 1]);
				this->cpu.memory.memory[begin + i] = output;
				//sprintf("%x\n", output[i]);
			}

			//step = 0;
			//SOL1_REGISTERS::set(this->cpu.registers.PCl, this->cpu.registers.PCh, 0x00);
			//SOL1_REGISTERS::set(this->cpu.registers.MARl, this->cpu.registers.MARh, 0x00);


			//this->cpu.memory.display(this->cpu.registers);
			//this->cpu.display_registers_lite();
			//runtime_counter = 0;
			debug = 1;
		}
		else if (input[0] == 'q' || input[0] == 'Q') {


			debugmenu_main_menu(this->hw_tty);
			int debug_status = debugmenu_main(this->cpu, this->hw_tty);
			if (debug_status == 1) { //SAME as R next IF
				this->cpu.DEBUG_MICROCODE = 0;
				this->cpu.DEBUG_REGISTERS = 0;
				this->cpu.DEBUG_ALU = 0;
				this->cpu.DEBUG_UFLAGS = 0;
				this->cpu.DEBUG_UART = 0;
				///
				run = 1;
				step = 0;
				microcodestep = 0;
				this->hw_tty.print("\n\n");
				this->hw_tty.print("###########################################\n");
				this->hw_tty.print("## Running Instructions ###################\n");
				this->hw_tty.print("###########################################\n");
			}
			else if (debug_status == 2) { //SAME as S next IF

				this->cpu.DEBUG_MICROCODE = 1;
				this->cpu.DEBUG_REGISTERS = 1;
				this->cpu.DEBUG_ALU = 1;

				debug = 1;
				run = 0;
				step = 0;
				microcodestep = 0;
				this->hw_tty.print("\n");

			}
			else if (debug_status == 0) {
				return;
			}
		}
		else if (input[0] == 'g' || input[0] == 'G') {

			///apagar
			this->cpu.DEBUG_MICROCODE = 0;
			this->cpu.DEBUG_REGISTERS = 0;
			this->cpu.DEBUG_ALU = 0;
			this->cpu.DEBUG_UFLAGS = 0;
			this->cpu.DEBUG_UART = 0;
			///
			run = 1;
			step = 0;
			microcodestep = 0;
			this->hw_tty.print("\n\n");
			this->hw_tty.print("###########################################\n");
			this->hw_tty.print("## Running Instructions ###################\n");
			this->hw_tty.print("###########################################\n");
		}
		else if (input[0] == 's' || input[0] == 'S') {

			this->cpu.DEBUG_MICROCODE = 1;
			this->cpu.DEBUG_REGISTERS = 1;
			this->cpu.DEBUG_ALU = 1;

			run = 0;
			step = 1;
			microcodestep = 0;
			this->hw_tty.print("\n\n");
			this->hw_tty.print("###########################################\n");
			this->hw_tty.print("## OpCode Step ############################\n");
			this->hw_tty.print("###########################################\n");
		}
		else if (input[0] == 'm' || input[0] == 'M') {
			run = 0;
			step = 0;
			microcodestep = 1;
			this->hw_tty.print("\n\n");
			this->hw_tty.print("###########################################\n");
			this->hw_tty.print("# Microcode Step ##########################\n");
			this->hw_tty.print("###########################################\n");
			this->hw_tty.print("***** REGISTERS\n");
			this->cpu.display_registers_lite(this->hw_tty);
			sprintf(str_out, "* IR: %02x\n", this->cpu.microcode.IR.value()); this->hw_tty.print(str_out);
			this->hw_tty.print("\n");
		}
		else if (input[0] == 'i' || input[0] == 'I') {
			run = 0;
			step = 1;
			microcodestep = 0;

			this->cpu.microcode.u_adder = this->cpu.microcode.old_u_ad_bus;
		}
		else if (input[0] == 'p' || input[0] == 'P') {
			debugmenu_main_reset_cpu(this->cpu, this->hw_tty);
			this->cpu.memory.reset();
			debug = 1;
		}

		else if (input[0] == 'o' || input[0] == 'O') {
			this->cpu.memory.display(this->cpu.registers, this->hw_tty);
			debug = 1;
		}
		else if (input[0] == 'r' || input[0] == 'R') {
			debugmenu_main_display_registers(this->cpu, this->hw_tty);
			//this->cpu.registers.display(this->cpu.registers);
			debug = 1;
		}
		else if (input[0] == '?') {
			trace_menu();
			debug = 1;
		}

		free(input);

		if (debug == 0)
			RunCPU(&runtime_counter);
		else
			debug = 0;

		oldPC = -1;
		oldOP = -1;

		if (run == 0 && step == 0 && microcodestep == 0)
			this->cpu.microcode.u_adder = 0x10;
	}
}