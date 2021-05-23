
#include "sol1_registers.h"




void sol1_registers_init(struct sol1_registers *regs) {
	//DATA REGISTERS
	regs->Ah = sol1_register_8bit_create(); // AX (16bit) Accumulator	(Ah/Al)
	regs->Al = sol1_register_8bit_create();
	regs->Bh = sol1_register_8bit_create(); // BX (16bit) Base		(Bh/Bl)
	regs->Bl = sol1_register_8bit_create();
	regs->Ch = sol1_register_8bit_create(); // CX (16bit) Counter		(Ch/Cl)
	regs->Cl = sol1_register_8bit_create();
	regs->Dh = sol1_register_8bit_create(); // DX (16bit) Data		(Dh/Dl)
	regs->Dl = sol1_register_8bit_create();
	regs->Gh = sol1_register_8bit_create(); // GX (16bit)	Gh/Gl	General Register(For scratch)
	regs->Gl = sol1_register_8bit_create();

	//Pointer Registers
	regs->BPh = sol1_register_8bit_create(); // BP (16bit) Base Pointer  (Used to manage stack frames)
	regs->BPl = sol1_register_8bit_create();
	regs->SPh = sol1_register_8bit_create(); // SP (16bit) Stack Pointer
	regs->SPl = sol1_register_8bit_create();
	regs->SSPh = sol1_register_8bit_create(); // SSP (16bit) Supervisor Stack Pointer = KSP
	regs->SSPl = sol1_register_8bit_create();

	//Index Registers
	regs->SIh = sol1_register_8bit_create(); // SI (16bit) Source index (Source address for string operations)
	regs->SIl = sol1_register_8bit_create();
	regs->DIh = sol1_register_8bit_create(); // DI (16bit) Destination Index (Destination address for string operations)
	regs->DIl = sol1_register_8bit_create();
	regs->PCh = sol1_register_8bit_create(); // PC (16bit) Program Counter
	regs->PCl = sol1_register_8bit_create();

	regs->TDRh = sol1_register_8bit_create(); // TDR (16bit) Temporary Data Register
	regs->TDRl = sol1_register_8bit_create();
	regs->PTB = sol1_register_8bit_create();  // PTB (8bit) = Page table base

	regs->MSWl = sol1_register_8bit_create();
	regs->MSWh = sol1_register_8bit_create();
}




void sol1_registers_destroy(struct sol1_registers *regs) {
	//DATA REGISTERS
	sol1_register_8bit_destroy(regs->Ah); // AX (16bit) Accumulator	(Ah/Al)
	sol1_register_8bit_destroy(regs->Al);
	sol1_register_8bit_destroy(regs->Bh); // BX (16bit) Base		(Bh/Bl)
	sol1_register_8bit_destroy(regs->Bl);
	sol1_register_8bit_destroy(regs->Ch); // CX (16bit) Counter		(Ch/Cl)
	sol1_register_8bit_destroy(regs->Cl);
	sol1_register_8bit_destroy(regs->Dh); // DX (16bit) Data		(Dh/Dl)
	sol1_register_8bit_destroy(regs->Dl);
	sol1_register_8bit_destroy(regs->Gh); // GX (16bit)	Gh/Gl	General Register(For scratch)
	sol1_register_8bit_destroy(regs->Gl);

	//Pointer Registers
	sol1_register_8bit_destroy(regs->BPh); // BP (16bit) Base Pointer  (Used to manage stack frames)
	sol1_register_8bit_destroy(regs->BPl);
	sol1_register_8bit_destroy(regs->SPh); // SP (16bit) Stack Pointer
	sol1_register_8bit_destroy(regs->SPl);
	sol1_register_8bit_destroy(regs->SSPh); // SSP (16bit) Supervisor Stack Pointer
	sol1_register_8bit_destroy(regs->SSPl);

	//Index Registers
	sol1_register_8bit_destroy(regs->SIh); // SI (16bit) Source index (Source address for string operations)
	sol1_register_8bit_destroy(regs->SIl);
	sol1_register_8bit_destroy(regs->DIh); // DI (16bit) Destination Index (Destination address for string operations)
	sol1_register_8bit_destroy(regs->DIl);
	sol1_register_8bit_destroy(regs->PCh); // PC (16bit) Program Counter
	sol1_register_8bit_destroy(regs->PCl);

	sol1_register_8bit_destroy(regs->TDRh); // TDR (16bit) Temporary Data Register
	sol1_register_8bit_destroy(regs->TDRl);
	sol1_register_8bit_destroy(regs->PTB);  // PTB (8bit) = Page table base

	sol1_register_8bit_destroy(regs->MSWl);
	sol1_register_8bit_destroy(regs->MSWh);
}


SOL1_DWORD sol1_registers_value(SOL1_REGISTER_8BIT *l, SOL1_REGISTER_8BIT *h) {

	return sol1_register_8bit_value(l) | (sol1_register_8bit_value(h) << 8);
}

void sol1_registers_set(SOL1_REGISTER_8BIT *l, SOL1_REGISTER_8BIT *h, SOL1_DWORD v) {

	sol1_register_8bit_set(l, (SOL1_BYTE)(v & 0b11111111));
	sol1_register_8bit_set(h, (SOL1_BYTE)((v >> 8) & 0b11111111));
}

void sol1_registers_reset(SOL1_REGISTER_8BIT *l, SOL1_REGISTER_8BIT *h) {
	sol1_register_8bit_reset(l);
	sol1_register_8bit_reset(h);
}
