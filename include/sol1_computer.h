#ifndef SOL1COMPUTER_H
#define SOL1COMPUTER_H

#include "sol1_bus.h"
#include "sol1_cpu.h"

#include "hw_uart.h"
#include "hw_rtc.h"
#include "hw_timer.h"
#include "hw_ide.h"
#include "hw_tty.h"
#include "tasm_opcode.h"

#include <unordered_map>
using namespace std;


class SOL1_COMPUTER {


public:
	SOL1_CPU cpu;
	SOL1_BUS bus;

	struct hw_rtc hw_rtc;
	struct hw_uart hw_uart;
	struct hw_ide hw_ide;
	struct hw_timer hw_timer;
	HW_TTY hw_tty;


	unordered_map<string, Tasm_Opcode> ht_opcodes;


	SOL1_BYTE apagar = 0;
	int gambi_ide = 0;
	int gambi_ide_total = 0;
	int gambi_ide_read = 0;
	int gambi_last_op = 0;

	
	int init();

	void run();




	SOL1_BYTE SOL1_COMPUTER::get_current_opcode();
	SOL1_BYTE SOL1_COMPUTER::get_current_opcode_cycle();
	void SOL1_COMPUTER::disassembly_current_opcode();
	void SOL1_COMPUTER::refresh_int();



	SOL1_BYTE SOL1_COMPUTER::buffer_rd();
	SOL1_BYTE SOL1_COMPUTER::buffer_wr();
	SOL1_BYTE SOL1_COMPUTER::buffer_mem_io();
	unsigned long SOL1_COMPUTER::read_address_bus();

	void SOL1_COMPUTER::mc_sequencer(long *runtime_counter);
	void SOL1_COMPUTER::RunCPU(long *runtime_counter);
};



#endif