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



#include "sol1_computer.h"

#include "sol1_alu.h" // aqui por enquaqnto
//#include "console_local.h"
#include "config.h"
#include "debugmenu_main.h"

#include "utils.h"

//#include "sol1_register_8bit.h"

//#include "ttl74series.h"

#include <math.h>





/// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//boot sequence: bios, boot, kernel, shell
/// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

using namespace std::chrono;




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

				for (i = tt.size - 2; i >= 0; i--)
					sprintf(line + strlen(line), "%02x", this->cpu.get_current_memory()[memADDR + i]);

				sprintf(line + strlen(line), ")");
			}

			sprintf(line + strlen(line), "\n");

			save_to_log(fa, line);
		}
		/*
		else if (current_opcode == 0 && sol1_registers_value(sol1_cpu.registers.PCl, sol1_cpu.registers.PCh) == 0)
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

		if (!check_byte_bit(this->cpu.registers.MSWl.value(), MSW_PAGING_EN)) {
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



	//printf("#################################################################################\n");
	//printf("# Runtime Counter: %d\n", (*runtime_counter)++);
	//printf("#################################################################################\n");


	////////////////////////////////////////////////////////////////////////////
	this->cpu.mc_seq_update();
	refresh_int();

	SOL1_BYTE current_opcode = get_current_opcode();
	SOL1_BYTE current_opcode_cycle = get_current_opcode_cycle();


	if ((this->cpu.DEBUG_LOG_OPCODE) && (current_opcode > 0))
		//if ((get_current_opcode() != 0 || (get_current_opcode() == 0 && (get_current_opcode_cycle(this->cpu) < 10 | get_current_opcode_cycle(this->cpu) > 14))))
		disassembly_current_opcode();


	if (!(this->cpu.rom.bkpt_opcode == 0 && this->cpu.rom.bkpt_cycle == 0) &&
		(current_opcode == this->cpu.rom.bkpt_opcode && current_opcode_cycle == this->cpu.rom.bkpt_cycle))
	{
		printf(" Microcode Breakpoint | Starting Opcode/Cycle:%02x%02x.\n", this->cpu.rom.bkpt_opcode, this->cpu.rom.bkpt_cycle);
		this->cpu.DEBUG_MICROCODE = 1;
		this->cpu.DEBUG_REGISTERS = 1;
		this->cpu.DEBUG_ALU = 1;
		step = 1;
		if (this->cpu.DEBUG_MICROCODE) {
			printf("***** MICROCODE\n");
			//printf("U-ADDRESS: ");  print_word_bin(this->cpu.microcode.u_ad_bus); printf("\n");		
			//printf("OPCODE: %02x (cycle %02x)\n", current_opcode, current_opcode_cycle);
			//printf("microcode: \n");
			printf("* U_FLAGS="); print_byte_bin(this->cpu.microcode.U_FLAGS.value()); printf("\n");
			this->cpu.rom.display_current_cycles_desc(current_opcode, current_opcode_cycle);
			printf("\n");
		}
		debugmenu_main(this->cpu);
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
					sprintf(log, "]]] READ RAM [%04x] = %02x \'\\0\'\n", mem_addr, this->bus.data_bus);
				else if (this->bus.data_bus == 0x09)
					sprintf(log, "]]] READ RAM [%04x] = %02x \'\\t\'\n", mem_addr, this->bus.data_bus);
				else if (this->bus.data_bus == 0x0a)
					sprintf(log, "]]] READ RAM [%04x] = %02x \'\\r\'\n", mem_addr, this->bus.data_bus);
				else if (this->bus.data_bus == 0x0d)
					sprintf(log, "]]] READ RAM [%04x] = %02x \'\\n\'\n", mem_addr, this->bus.data_bus);
				else
					sprintf(log, "]]] READ RAM [%04x] = %02x \'%c\'\n", mem_addr, this->bus.data_bus, this->bus.data_bus);
				save_to_log(fa, log);
			}
		}
		else if (mem_addr >= 0xFF80 && mem_addr <= 0xFFFF) {

			//RTC I/O bank = FFA0 to FFAF
			if (mem_addr >= 0xFFA0 && mem_addr <= 0xFFAF) {
				this->bus.data_bus = this->hw_rtc.data[mem_addr - 0xFFA0];

				if ((this->cpu.DEBUG_RTC) > 0) {

					char log_rtc[255];
					hw_rtc_print(&this->hw_rtc, "READ ", (mem_addr - 0xFFA0), log_rtc);
					save_to_log(fa, log_rtc);
				}
			}

			//TIMER FFE0 - FFE3
			else if (mem_addr >= 0xFFE0 && mem_addr <= 0xFFE3) {
				this->bus.data_bus = this->hw_timer.data[mem_addr - 0xFFE0];

				if ((this->cpu.DEBUG_TIMER) > 0) {

					char log_timer[255];
					hw_timer_print(&this->hw_timer, "READ ", (mem_addr - 0xFFE0), log_timer);
					save_to_log(fa, log_timer);
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
					hw_uart_print(&this->hw_uart, "READ", (mem_addr - 0xFF80), log_uart);
					save_to_log(fa, log_uart);
				}

			}


			if (mem_addr >= 0xFFD0 && mem_addr <= 0xFFD7) {


				if (this->hw_ide.data[7] == 0b00001000 && mem_addr - 0xFFD0 == 0) {

					this->gambi_ide_total = this->hw_ide.data[2];

					unsigned long sec_address_lba = this->hw_ide.data[3];
					sec_address_lba = sec_address_lba | ((unsigned long)this->hw_ide.data[4]) << 8;
					sec_address_lba = sec_address_lba | ((unsigned long)this->hw_ide.data[5]) << 16;
					sec_address_lba = sec_address_lba | ((unsigned long)(this->hw_ide.data[6] & 0b00001111)) << 24;

					unsigned long sec_address_byte = sec_address_lba * 512;

					if (sec_address_byte < SOL1_IDE_MEMORY_SIZE) {
						this->hw_ide.data[0] = this->hw_ide.memory[sec_address_byte + this->gambi_ide_read];

						this->gambi_ide_read++;


						if (this->gambi_ide_read > this->gambi_ide_total * 512)
						{
							this->gambi_ide_total = 0;
							this->gambi_ide_read = 0;

							this->hw_ide.data[7] = 0x00;
						}
					}
					else {
						this->gambi_ide_total = 0;
						this->gambi_ide_read = 0;

						this->hw_ide.data[7] = 0x24;
					}
				}

				//hw_ide_read(this->cpu, uart_out);
				this->bus.data_bus = this->hw_ide.data[mem_addr - 0xFFD0];


				if ((this->cpu.DEBUG_IDE & get_current_opcode()) > 0) {
					char log_ide[255];
					hw_ide_print(&this->hw_ide, "READ ", (mem_addr - 0xFFD0), log_ide);
					save_to_log(fa, log_ide);
				}

			}

		}
	}

	////////////////////////////////////////////////////////////////////////////
	// W = X
	this->bus.w_bus = this->bus.w_bus_refresh(this->cpu.registers, this->cpu.microcode.mccycle.panel_regsel,
		this->cpu.alu.alu_a_src, this->cpu.microcode.mccycle.display_reg_load && this->cpu.display_reg_load,
		this->cpu.microcode.mccycle.int_vector, this->cpu.registers.INT_MASKS.value(), this->cpu.microcode.mccycle.int_status);

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

	this->cpu.microcode.u_flags(this->cpu.alu, this->bus.z_bus, this->cpu.DEBUG_UFLAGS);
	this->cpu.microcode.update_final_condition(this->cpu.registers);



	SOL1_BYTE alu_result = ALU_EXEC(&this->cpu.alu, this->bus.x_bus, this->bus.y_bus,
		this->cpu.microcode.u_cf,
		get_byte_bit(this->cpu.registers.MSWh.value(), MSW_CF),
		this->cpu.microcode.mccycle.shift_src,
		this->cpu.microcode.mccycle.zbus_out_src, this->cpu.DEBUG_ALU);

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
					sprintf(log, "]]] WRITE RAM [%04x] = %02x \'\\0'\n", mem_addr, this->bus.data_bus);
				else if (this->bus.data_bus == 0x09)
					sprintf(log, "]]] WRITE RAM [%04x] = %02x \'\\t'\n", mem_addr, this->bus.data_bus);
				else if (this->bus.data_bus == 0x0a)
					sprintf(log, "]]] WRITE RAM [%04x] = %02x \'\\r\'\n", mem_addr, this->bus.data_bus);
				else if (this->bus.data_bus == 0x0d)
					sprintf(log, "]]] WRITE RAM [%04x] = %02x \'\\n\'\n", mem_addr, this->bus.data_bus);
				else
					sprintf(log, "]]] WRITE RAM [%04x] = %02x \'%c\'\n", mem_addr, this->bus.data_bus, this->bus.data_bus);
				save_to_log(fa, log);
			}

			this->cpu.get_current_memory()[mem_addr] = this->bus.data_bus;

		}
		else if (mem_addr >= 0xFF80 && mem_addr <= 0xFFFF) {

			//RTC I/O bank = FFA0 to FFAF
			if (mem_addr >= 0xFFA0 && mem_addr <= 0xFFAF) {
				this->hw_rtc.data[mem_addr - 0xFFA0] = this->bus.data_bus;


				if ((this->cpu.DEBUG_RTC) > 0) {
					char log_rtc[255];
					hw_rtc_print(&this->hw_rtc, "WRITE", (mem_addr - 0xFFA0), log_rtc);
					save_to_log(fa, log_rtc);
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
					hw_timer_print(&this->hw_timer, "WRITE", (mem_addr - 0xFFE0), log_timer);
					save_to_log(fa, log_timer);
				}
			}

			//UART SERVICES INTERRUPT
			else if (mem_addr >= 0xFF80 && mem_addr <= 0xFF85) {

				hw_uart_get_lsr(&this->hw_uart);

				if (mem_addr - 0xFF80 == 0 && (this->hw_uart.data[5] == 0x20 || this->hw_uart.data[5] == 0x01)) {
					hw_uart_send(&this->hw_uart, this->bus.data_bus);

					int i;
					for (i = 0; i < 10; i++) {
						if (this->hw_tty.clients[i].running == 1) {
							HW_TTY::net_data *net_data = (HW_TTY::net_data*)malloc(sizeof(HW_TTY::net_data));
							net_data->data = this->bus.data_bus;
							queue_insert(this->hw_tty.clients[i].uart_out, net_data);

						}
					}

				}
				else
					this->hw_uart.data[mem_addr - 0xFF80] = this->bus.data_bus;

				if ((this->cpu.DEBUG_UART & get_current_opcode()) > 0) {
					//hw_uart_write(this->cpu, uart_out, this->bus.data_bus);
					char log_uart[255];
					hw_uart_print(&this->hw_uart, "WRITE", (mem_addr - 0xFF80), log_uart);
					save_to_log(fa, log_uart);
				}

			}

			if (mem_addr >= 0xFFD0 && mem_addr <= 0xFFD7) {

				this->hw_ide.data[mem_addr - 0xFFD0] = this->bus.data_bus;

				if ((this->cpu.DEBUG_IDE & get_current_opcode()) > 0) {
					char log_ide[255];
					hw_ide_print(&this->hw_ide, "WRITE", (mem_addr - 0xFFD0), log_ide);
					save_to_log(fa, log_ide);
				}




				if (this->hw_ide.data[7] == 0b00001000 && mem_addr - 0xFFD0 == 0) {

					this->gambi_ide_total = this->hw_ide.data[2];

					unsigned long sec_address_lba = this->hw_ide.data[3];
					sec_address_lba = sec_address_lba | ((unsigned long)this->hw_ide.data[4]) << 8;
					sec_address_lba = sec_address_lba | ((unsigned long)this->hw_ide.data[5]) << 16;
					sec_address_lba = sec_address_lba | ((unsigned long)(this->hw_ide.data[6] & 0b00001111)) << 24;

					unsigned long sec_address_byte = sec_address_lba * 512;

					if (sec_address_byte < SOL1_IDE_MEMORY_SIZE) {
						this->hw_ide.memory[sec_address_byte + this->gambi_ide_read] = this->hw_ide.data[0];

						this->gambi_ide_read++;


						if (this->gambi_ide_read > this->gambi_ide_total * 512)
						{
							this->gambi_ide_total = 0;
							this->gambi_ide_read = 0;

							this->hw_ide.data[7] = 0x00;

							hw_ide_save_disk(this->hw_ide.memory);
						}
					}
					else {
						this->gambi_ide_total = 0;
						this->gambi_ide_read = 0;

						this->hw_ide.data[7] = 0x34;
					}
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
					this->gambi_ide_total = 0;
					this->gambi_ide_read = 0;

				}
				else if (this->hw_ide.data[7] == 0x30) { // write sector cmd
					this->hw_ide.data[7] = 0b00001000;
					this->gambi_ide_total = 0;
					this->gambi_ide_read = 0;

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
		SOL1_BYTE oldStatus = this->cpu.registers.MSWl.value();
		this->cpu.registers.MSWl.set(this->bus.z_bus);
	}



	///////////////////////////////////////////////////////////////////////////
	// READ DATA
	//DATA REGISTERS
	if (this->cpu.microcode.mccycle.ah_wrt == 0x00) this->cpu.registers.Ah.set(this->bus.z_bus);
	if (this->cpu.microcode.mccycle.al_wrt == 0x00) this->cpu.registers.Al.set(this->bus.z_bus);

	if (this->cpu.microcode.mccycle.bh_wrt == 0x00) this->cpu.registers.Bh.set(this->bus.z_bus);
	if (this->cpu.microcode.mccycle.bl_wrt == 0x00) this->cpu.registers.Bl.set(this->bus.z_bus);

	if (this->cpu.microcode.mccycle.ch_wrt == 0x00) this->cpu.registers.Ch.set(this->bus.z_bus);
	if (this->cpu.microcode.mccycle.cl_wrt == 0x00) this->cpu.registers.Cl.set(this->bus.z_bus);

	if (this->cpu.microcode.mccycle.dh_wrt == 0x00) this->cpu.registers.Dh.set(this->bus.z_bus);
	if (this->cpu.microcode.mccycle.dl_wrt == 0x00) this->cpu.registers.Dl.set(this->bus.z_bus);

	/*
	if (get_byte_bit(this->cpu.registers.MSWl.value(), MSW_PAGING_EN) == 0) {
		if (this->cpu.microcode.mccycle.dh_wrt == 0x00) this->cpu.registers.Dh.set(this->bus.z_bus);
		if (this->cpu.microcode.mccycle.dl_wrt == 0x00) this->cpu.registers.Dl.set(this->bus.z_bus);
	}
	else {
		if (this->cpu.microcode.mccycle.dh_wrt == 0x00) this->cpu.registers.DIh.set(this->bus.z_bus);
		if (this->cpu.microcode.mccycle.dl_wrt == 0x00) this->cpu.registers.DIl.set(this->bus.z_bus);
	}
	*/
	if (this->cpu.microcode.mccycle.gh_wrt == 0x00) this->cpu.registers.Gh.set(this->bus.z_bus);
	if (this->cpu.microcode.mccycle.gl_wrt == 0x00) this->cpu.registers.Gl.set(this->bus.z_bus);

	//Pointer Registers
	if (this->cpu.microcode.mccycle.bph_wrt == 0x00) this->cpu.registers.BPh.set(this->bus.z_bus);
	if (this->cpu.microcode.mccycle.bpl_wrt == 0x00) this->cpu.registers.BPl.set(this->bus.z_bus);



	if (!check_byte_bit(this->cpu.registers.MSWl.value(), MSW_CPU_MODE)) {

		if ((this->cpu.microcode.mccycle.sph_wrt == 0x00) || (this->cpu.microcode.mccycle.ssph_wrt == 0x00)) {
			this->cpu.registers.SPh.set(this->bus.z_bus);
			this->cpu.registers.SSPh.set(this->bus.z_bus);
		}
		if ((this->cpu.microcode.mccycle.spl_wrt == 0x00) || this->cpu.microcode.mccycle.sspl_wrt == 0x00) {
			this->cpu.registers.SPl.set(this->bus.z_bus);
			this->cpu.registers.SSPl.set(this->bus.z_bus);
		}
	}
	else {
		if (this->cpu.microcode.mccycle.sph_wrt == 0x00) this->cpu.registers.SPh.set(this->bus.z_bus);
		if (this->cpu.microcode.mccycle.spl_wrt == 0x00) this->cpu.registers.SPl.set(this->bus.z_bus);


		if (this->cpu.microcode.mccycle.ssph_wrt == 0x00) this->cpu.registers.SSPh.set(this->bus.z_bus);
		if (this->cpu.microcode.mccycle.sspl_wrt == 0x00) this->cpu.registers.SSPl.set(this->bus.z_bus);
	}

	//Index Registers
	if (this->cpu.microcode.mccycle.sih_wrt == 0x00) this->cpu.registers.SIh.set(this->bus.z_bus);
	if (this->cpu.microcode.mccycle.sil_wrt == 0x00) this->cpu.registers.SIl.set(this->bus.z_bus);

	if (this->cpu.microcode.mccycle.dih_wrt == 0x00) this->cpu.registers.DIh.set(this->bus.z_bus);
	if (this->cpu.microcode.mccycle.dil_wrt == 0x00) this->cpu.registers.DIl.set(this->bus.z_bus);

	if (this->cpu.microcode.mccycle.pch_wrt == 0x00) this->cpu.registers.PCh.set(this->bus.z_bus);
	if (this->cpu.microcode.mccycle.pcl_wrt == 0x00) this->cpu.registers.PCl.set(this->bus.z_bus);

	if (this->cpu.microcode.mccycle.tdrh_wrt == 0x00) this->cpu.registers.TDRh.set(this->bus.z_bus);
	if (this->cpu.microcode.mccycle.tdrl_wrt == 0x00) this->cpu.registers.TDRl.set(this->bus.z_bus);

	if (this->cpu.microcode.mccycle.ptb_wrt == 0x00) this->cpu.registers.PTB.set(this->bus.z_bus);

	/////////////////////////////////////////////////////////////////////////////
	if (this->cpu.microcode.mccycle.mask_flags_wrt == 0x00) this->cpu.registers.INT_MASKS.set(this->bus.z_bus);
	/////////////////////////////////////////////////////////////////////////////
	// SET MDR
	 //IC7 //IC24 //IC19 //IC183

	if (this->cpu.microcode.mccycle.mdrl_wrt == 0x00) this->cpu.registers.MDRl.set(this->cpu.microcode.mccycle.mdr_in_src == 0x00 ? this->bus.z_bus : this->bus.data_bus);
	if (this->cpu.microcode.mccycle.mdrh_wrt == 0x00) this->cpu.registers.MDRh.set(this->cpu.microcode.mccycle.mdr_in_src == 0x00 ? this->bus.z_bus : this->bus.data_bus);
	////////////////////////////////////////////////////////////////////////////
	//MEMORY SET MAR

	if (this->cpu.microcode.mccycle.marl_wrt == 0x00) {
		//IC131 //IC128			
		if (this->cpu.microcode.mccycle.mar_in_src == 0x00)
			this->cpu.registers.MARl.set(this->bus.z_bus);
		else
			this->cpu.registers.MARl.set(this->cpu.registers.PCl.value());
	}

	if (this->cpu.microcode.mccycle.marh_wrt == 0x00) {
		//IC129 //IC132			
		if (this->cpu.microcode.mccycle.mar_in_src == 0x00)
			this->cpu.registers.MARh.set(this->bus.z_bus);
		else
			this->cpu.registers.MARh.set(this->cpu.registers.PCh.value());
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
		printf("***** BUS\n");
		printf("* u_ad_bus    : "); print_word_bin(this->cpu.microcode.u_ad_bus); printf("\n");
		printf("* address bus : "); print_byte_bin((SOL1_BYTE)(read_address_bus() >> 16)); printf(" ");  print_word_bin(read_address_bus()); printf("\n");
		printf("*  data_bus   |");
		printf("    k_bus    |");
		printf("    w_bus    |");
		printf("    x_bus    |");
		printf("    y_bus    |");
		printf("    z_bus    ");
		printf("\n");
		printf("  "); print_byte_bin(this->bus.data_bus);
		printf(" | "); print_byte_bin(this->bus.k_bus);
		printf(" | "); print_byte_bin(this->bus.w_bus);
		printf(" | "); print_byte_bin(this->bus.x_bus);
		printf(" | "); print_byte_bin(this->bus.y_bus);
		printf(" | "); print_byte_bin(this->bus.z_bus);
		printf("\n");
		printf("\n");
	}

	if (this->cpu.DEBUG_MEMORY) {
		//printf("***** MEMORY\n");
		this->cpu.memory.display(this->cpu.registers);
	}

	if (this->cpu.DEBUG_REGISTERS) {
		printf("***** REGISTERS\n");
		this->cpu.display_registers();

	}


	//CLOCK HIGH





	this->cpu.refresh_MSWh_ZF(this->bus.z_bus, this->cpu.microcode.mccycle.zf_in_src);
	this->cpu.refresh_MSWh_CF(this->bus.z_bus, this->cpu.microcode.mccycle.cf_in_src);
	this->cpu.refresh_MSWh_SF(this->bus.z_bus, this->cpu.microcode.mccycle.sf_in_src);
	this->cpu.refresh_MSWh_OF(this->bus.z_bus, this->cpu.microcode.u_sf, this->cpu.microcode.mccycle.of_in_src);

	this->cpu.microcode.u_flags(this->cpu.alu, this->bus.z_bus, this->cpu.DEBUG_UFLAGS);
	this->cpu.microcode.update_final_condition(this->cpu.registers);

	this->cpu.microcode.u_adder_refresh(this->cpu.microcode.mccycle.next, this->cpu.microcode.mccycle.final_condition);











	if (this->cpu.DEBUG_UADDER) {
		printf("***** U_ADDER\n");
		this->cpu.microcode.display_u_adder(this->cpu.microcode.mccycle.next, this->cpu.microcode.mccycle.final_condition);
		printf("\n");
	}

	if (!(this->cpu.rom.bkpt_opcode == 0 && this->cpu.rom.bkpt_cycle == 0) &&
		(current_opcode == this->cpu.rom.bkpt_opcode &&
			current_opcode_cycle == this->cpu.rom.bkpt_cycle))
	{
		printf(" Microcode Breakpoint | Opcode/Cycle:%02x%02x Finished.\n", this->cpu.rom.bkpt_opcode, this->cpu.rom.bkpt_cycle);
		debugmenu_main(this->cpu);
	}

	if ((this->cpu.BKPT != 0xFFFF) && (SOL1_REGISTERS::value(this->cpu.registers.MARl, this->cpu.registers.MARh) == this->cpu.BKPT
		|| SOL1_REGISTERS::value(this->cpu.registers.PCl, this->cpu.registers.PCh) == this->cpu.BKPT))
	{
		this->cpu.DEBUG_MICROCODE = 1;
		this->cpu.DEBUG_REGISTERS = 1;
		this->cpu.DEBUG_ALU = 1;
		step = 1;
		if (this->cpu.DEBUG_MICROCODE) {
			printf("\n***** MICROCODE\n");
			//printf("U-ADDRESS: ");  print_word_bin(this->cpu.microcode.u_ad_bus); printf("\n");		
			//printf("OPCODE: %02x (cycle %02x)\n", current_opcode, current_opcode_cycle);
			//printf("microcode: \n");
			printf("* U_FLAGS="); print_byte_bin(this->cpu.microcode.U_FLAGS.value()); printf("\n");
			this->cpu.rom.display_current_cycles_desc(current_opcode, current_opcode_cycle);
			printf("\n");
		}

		printf(" Memory Breakpoint | Starting Address:%04x.\n", this->cpu.BKPT);
		debugmenu_main(this->cpu);
	}
	/*
	else if ((this->cpu.BKPT != 0xFFFF) && (sol1_registers_value(this->cpu.registers.MARl, this->cpu.registers.MARh) == this->cpu.BKPT
		|| sol1_registers_value(this->cpu.registers.PCl, this->cpu.registers.PCh) == this->cpu.BKPT))
	{
		printf(" Memory Breakpoint | Address:%04x Finished.\n", this->cpu.BKPT);
		debugmenu_main(this->cpu);
	}
	*/


	hw_timer_tick_c0(&this->hw_timer);

	this->bus.data_bus = 0;
	this->bus.k_bus = 0;
	this->bus.w_bus = 0;
	this->bus.x_bus = 0;
	this->bus.y_bus = 0;
	this->bus.z_bus = 0;

}


void SOL1_COMPUTER::RunCPU(long *runtime_counter) {

	while (1) {



		mc_sequencer(runtime_counter);

		SOL1_BYTE current_opcode = get_current_opcode();
		SOL1_BYTE current_opcode_cycle = get_current_opcode_cycle();



		if (!this->cpu.SERVER) {



			if (kbhit()) {
				char dddd = getch();
				if (dddd == 0x1b)
					return;

				/*
				if (dddd == '0') {// || 1 ) {

					this->cpu.DEBUG_MICROCODE = 1;
					this->cpu.DEBUG_UADDRESSER = INI_DEBUG_UADDRESSER;
					this->cpu.DEBUG_UADDER = INI_DEBUG_UADDER;
					this->cpu.DEBUG_UFLAGS = INI_DEBUG_UFLAGS;
					this->cpu.DEBUG_REGISTERS = 1;

					this->cpu.DEBUG_BUSES = INI_DEBUG_BUSES;
					this->cpu.DEBUG_ALU = 1;
					this->cpu.DEBUG_RDMEM = INI_DEBUG_RDMEM;
					this->cpu.DEBUG_WRMEM = INI_DEBUG_WRMEM;
					this->cpu.DEBUG_MEMORY = INI_DEBUG_MEMORY;
					this->cpu.DEBUG_UART = 1;
					this->cpu.DEBUG_LITE = 0;
					step = 1;
				}
				*/

				hw_uart_receive(&this->hw_uart, dddd);
				this->cpu.microcode.mccycle.int_request = 0x01;
			}
		}

		if (this->hw_uart.data[5] == 0xFF || this->hw_uart.data[5] == 0x20)
			if (!queue_empty(this->hw_uart.uart_in))
				hw_uart_write(&this->hw_uart);





		if (this->cpu.DEBUG_LITE && (current_opcode != oldOP || SOL1_REGISTERS::value(this->cpu.registers.PCl, this->cpu.registers.PCh) != oldPC)) {

			SOL1_BYTE cur_opcode = current_opcode;
			SOL1_BYTE cur_cycle = current_opcode_cycle;
			SOL1_BYTE pc_opcode = this->cpu.get_current_memory()[SOL1_REGISTERS::value(this->cpu.registers.PCl, this->cpu.registers.PCh)];

			if (!(cur_opcode == 0x00 && cur_cycle == 0x10)) {
				char str[100];
				sprintf(str, "# Opcode=%02x:%s", cur_opcode, &this->cpu.rom.rom_desc[0x400000 + (cur_opcode * 256)]);
				printf("%s", rightpad(str, 40));

				printf(" | ");
				printf("Cycle: % 02x", cur_cycle);
				printf(" | ");

				printf("PC: % 04x", SOL1_REGISTERS::value(this->cpu.registers.PCl, this->cpu.registers.PCh));
				printf(" | ");
				/*
				printf("@PC=%02x:%s\n", pc_opcode, &this->cpu.rom.rom_desc[0x400000 + (pc_opcode * 256)]);
				*/
				printf("MEM:%02x%02x",
					this->cpu.get_current_memory()[SOL1_REGISTERS::value(this->cpu.registers.MARl, this->cpu.registers.MARh) + 1],
					this->cpu.get_current_memory()[SOL1_REGISTERS::value(this->cpu.registers.MARl, this->cpu.registers.MARh)]
				);

				printf("\n");

			}
			oldOP = cur_opcode;
			oldPC = SOL1_REGISTERS::value(this->cpu.registers.PCl, this->cpu.registers.PCh);
		}

		if (this->cpu.microcode.MUX(this->cpu.registers) == 0x02 && step == 1) {

			if (!this->cpu.DEBUG_LITE) {
				printf("###########################################\n");
				printf("## End OpStep on Opcode/Cycle:%02x%02x. #######\n", current_opcode, current_opcode_cycle);
				printf("###########################################\n");
			}
			return;
		}
		else if (step == 1 && !this->cpu.DEBUG_LITE) {
			printf("###########################################\n");
		}
		else if (microcodestep == 1) {
			if (!this->cpu.DEBUG_LITE) {
				printf("###########################################\n");
				printf("## End MicroStep on Opcode/Cycle:%02x%02x. ####\n", current_opcode, current_opcode_cycle);
				printf("###########################################\n");
			}
			return;
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


	char buff[255];
	//	char buff2[255];
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
			this->ht_opcodes[strlower(_opcode)] = Tasm_Opcode(_opcode, _desc, _size);
		}
	}

	fclose(fp);

	this->cpu.init();

	fa = fopen("File1.txt", "w");

	hw_uart_init(&this->hw_uart);
	hw_rtc_init(&this->hw_rtc);
	hw_rtc_start_clock(&this->hw_rtc);
	hw_timer_init(&this->hw_timer);
	hw_ide_init(&this->hw_ide);
	this->hw_tty.init(this->cpu, &this->hw_uart);


	int i;
	long size = 0;

	char * buf = loadfile("bios.obj", &size);
	//char * buf = loadfile("bios_paulo.obj", &size);
	if (buf == NULL)
		return 0;

	for (i = 0; i < size; i++) {
		this->cpu.memory.memory[i] = buf[i];
	}


	hw_ide_load_disk(this->hw_ide.memory);
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


	//init bus
	this->bus.data_bus = 0b00000000;
	this->bus.k_bus = 0b00000000; // input pra alu x e y
	this->bus.w_bus = 0b00000000; // input pra alu x e y
	this->bus.x_bus = 0b00000000; //alu entrada
	this->bus.y_bus = 0b00000000; //alu entrada
	this->bus.z_bus = 0b00000000; //alu saida


	if (this->cpu.SERVER) {
		this->hw_tty.start_server();
	}

	return 1;
}




void SOL1_COMPUTER::run() {




	long runtime_counter = 0;

	//cpu_print(&z80);
	int run = 0;
	int debug = 0;

	int start = 1;
	while (1) {
		char *input = (char*)malloc(sizeof(char) * 257);

		//sol1_cpu_memory_display(&sol1_cpu);
		//sol1_cpu_display_registers_lite(&sol1_cpu);
		//printf("Flags: "); mswh_flags_desc(&sol1_cpu); printf("\n");
		//printf("Status: "); mswl_status_desc(&sol1_cpu); printf("\n");

		if (start == 1) {

			input[0] = 'n';
			input[1] = '\n';
			start = 0;
		}
		else {
			printf("TRACE> ");
			scanf("%s", input);
		}

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
				this->cpu.memory.memory[i] = output;
				//printf("%x\n", output[i]);
			}

			step = 1;
			SOL1_REGISTERS::set(this->cpu.registers.PCl, this->cpu.registers.PCh, 0x00);
			SOL1_REGISTERS::set(this->cpu.registers.MARl, this->cpu.registers.MARh, 0x00);


			this->cpu.memory.display(this->cpu.registers);
			this->cpu.display_registers_lite();
			runtime_counter = 0;
		}
		else if (input[0] == 'n' || input[0] == 'N') {


			debugmenu_main_menu();
			int debug_status = debugmenu_main(this->cpu);
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
				printf("\n\n");
				printf("###########################################\n");
				printf("## Running Instructions ###################\n");
				printf("###########################################\n");
			}
			else if (debug_status == 2) { //SAME as S next IF

				this->cpu.DEBUG_MICROCODE = 1;
				this->cpu.DEBUG_REGISTERS = 1;
				this->cpu.DEBUG_ALU = 1;

				debug = 1;
				run = 0;
				step = 0;
				microcodestep = 0;
				printf("\n");

			}
			else if (debug_status == 0) {
				return;
			}
		}
		else if (input[0] == 'r' || input[0] == 'R') {

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
			printf("\n\n");
			printf("###########################################\n");
			printf("## Running Instructions ###################\n");
			printf("###########################################\n");
		}
		else if (input[0] == 's' || input[0] == 'S') {

			this->cpu.DEBUG_MICROCODE = 1;
			this->cpu.DEBUG_REGISTERS = 1;
			this->cpu.DEBUG_ALU = 1;

			run = 0;
			step = 1;
			microcodestep = 0;
			if (!this->cpu.DEBUG_LITE) {
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
			if (!this->cpu.DEBUG_LITE) {
				printf("\n\n");
				printf("###########################################\n");
				printf("# Microcode Step ##########################\n");
				printf("###########################################\n");
				printf("***** REGISTERS\n");
				this->cpu.display_registers_lite();;
				printf("* IR: %02x\n", this->cpu.microcode.IR.value());
				printf("\n");
			}

		}
		else if (input[0] == 'b' || input[0] == 'B') {
			run = 0;
			step = 1;
			microcodestep = 0;

			this->cpu.microcode.u_adder = this->cpu.microcode.old_u_ad_bus;
		}
		else if (input[0] == 'p' || input[0] == 'P') {
			this->cpu.microcode.IR.set(0x00);
			this->cpu.reset();
			this->cpu.memory.reset();
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