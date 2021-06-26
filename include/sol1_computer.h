#ifndef SOL1COMPUTER_H
#define SOL1COMPUTER_H
#include "config.h"
#include "sol1_bus.h"
#include "sol1_cpu.h"

#include "hw_uart.h"
#include "hw_rtc.h"
#include "hw_timer.h"
#include "hw_ide.h"
#include "hw_tty.h"
#include "hw_web.h"
#include "tasm_opcode.h"

#include <unordered_map>
using namespace std;


class SOL1_COMPUTER {

private:
	void trace_menu();
	
public:
	SOL1_CPU cpu;
	SOL1_BUS bus;

	struct hw_rtc hw_rtc;
	struct hw_uart hw_uart;
	struct hw_ide hw_ide;
	struct hw_timer hw_timer;

	HW_TTY hw_tty;
	HW_WEB hw_web;

	unordered_map<string, Tasm_Opcode> ht_opcodes;


	//SOL1_BYTE apagar = 0;
	//int gambi_ide = 0;
	//int gambi_ide_total = 0;
	//int gambi_ide_read = 0;
	//int gambi_last_op = 0;

	
	int init();

	void run();




	SOL1_BYTE get_current_opcode();
	SOL1_BYTE get_current_opcode_cycle();
	void disassembly_current_opcode();
	void refresh_int();



	SOL1_BYTE buffer_rd();
	SOL1_BYTE buffer_wr();
	SOL1_BYTE buffer_mem_io();
	unsigned long read_address_bus();

	void mc_sequencer(long *runtime_counter);
	void RunCPU(long *runtime_counter);
};



#endif