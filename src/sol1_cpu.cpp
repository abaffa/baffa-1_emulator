#include "sol1_cpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include <time.h>

#define DEBUG_OPCODE 1

void sol1_cpu_init(struct sol1_cpu* sol1_cpu)
{
    memset(sol1_cpu, 0, sizeof(struct sol1_cpu));
	sol1_memory_init(&sol1_cpu->memory);

	sol1_cpu_reset(sol1_cpu);
}

void sol1_cpu_reset(struct sol1_cpu *sol1_cpu)
{
	//DATA REGISTERS
	sol1_cpu->registers.A = 0x0000; // AX (16bit) Accumulator	(Ah/Al)
	sol1_cpu->registers.B = 0x0000; // BX (16bit) Base		(Bh/Bl)
	sol1_cpu->registers.C = 0x0000; // CX (16bit) Counter		(Ch/Cl)
	sol1_cpu->registers.D = 0x0000; // DX (16bit) Data		(Dh/Dl)
	sol1_cpu->registers.G = 0x0000; // GX (16bit)	Gh/Gl	General Register(For scratch)

	//Pointer Registers
	sol1_cpu->registers.BP = 0x0000; // BP (16bit) Base Pointer  (Used to manage stack frames)
	sol1_cpu->registers.SP = 0x0000; // SP (16bit) Stack Pointer

	//Index Registers
	sol1_cpu->registers.SI = 0x0000; // SI (16bit) Source index (Source address for string operations)
	sol1_cpu->registers.DI = 0x0000; // DI (16bit) Destination Index (Destination address for string operations)
	sol1_cpu->registers.PC = 0x0000; // PC (16bit) Program Counter

	sol1_cpu->registers.TDR = 0x0000; // TDR (16bit) Temporary Data Register
	sol1_cpu->registers.SSP = 0x0000; // SSP (16bit) Supervisor Stack Pointer
	sol1_cpu->registers.PTB = 0x00;  // PTB (8bit) = Page table base

	//JumpZ80(z80, z80->registers.PC);


	sol1_cpu->BKPT = 0xFFFF; // Breakpoint
}

void sol1_cpu_display_registers(struct sol1_cpu *sol1_cpu) {
	printf(" DATA REGISTERS\n");
	printf(" *A=%04x", sol1_cpu->registers.A);
	printf(" | ");
	printf("B=%04x", sol1_cpu->registers.B);
	printf(" | ");
	printf("C=%04x", sol1_cpu->registers.C);
	printf(" | ");
	printf("D=%04x", sol1_cpu->registers.D);
	printf(" | ");
	printf("G=%04x", sol1_cpu->registers.G);
	printf("\n\n");

	printf(" POINTER REGISTERS\n");
	printf(" *BP=%04x", sol1_cpu->registers.BP);
	printf(" | ");
	printf("SP=%04x", sol1_cpu->registers.SP);
	printf("\n\n");

	printf(" INDEX REGISTERS\n");
	printf(" *SI=%04x", sol1_cpu->registers.SI);
	printf(" | ");
	printf("DI=%04x", sol1_cpu->registers.DI);
	printf(" | ");
	printf("PC=%04x", sol1_cpu->registers.PC);
	printf(" | ");
	printf("@PC={DISM BYTE}");
	printf("\n\n");

	printf(" SPECIAL REGISTERS\n");
	printf(" *TDR=%04x", sol1_cpu->registers.TDR);
	printf(" | ");
	printf("SSP=%04x", sol1_cpu->registers.SSP);
	printf(" | ");
	printf("PTB=%02x", sol1_cpu->registers.PTB);
	printf("\n\n");

	printf(" MEMORY REGISTERS\n");
	printf(" * MAR=%04x", sol1_cpu->memory.MAR);
	printf(" | ");
	printf("MDR=%02x", sol1_cpu->memory.MDR);
	printf("\n");
}