#ifndef SOL1REGISTERS_H
#define SOL1REGISTERS_H

#include "config.h"
#include "sol1_register_8bit.h"

// FLAG - msw-h - bits
#define MSW_ZF 0x00 // ZeroFlag
#define MSW_CF 0x01 // Carry Flag
#define MSW_SF 0x02 // Overflow Flag
#define MSW_OF 0x03 // Sign Flag
#define MSW_12 0x04
#define MSW_13 0x05
#define MSW_14 0x06
#define MSW_15 0x07

// STATUS FLAGS - msw-l bits
#define MSW_DMA_ACK 0x00
#define MSW_INTERRUPT_ENABLE 0x01
#define MSW_CPU_MODE 0x02
#define MSW_PAGING_EN 0x03
#define MSW_HALT 0x04
#define MSW_DISPLAY_REG_LOAD 0x05
//#define MSW_14 0x06
#define MSW_DIR 0x07

class SOL1_REGISTERS
{

public:
	//General Purpose Registers
	//DATA REGISTERS
	SOL1_REGISTER_8BIT Ah; // AX (16bit) Accumulator	(Ah/Al)
	SOL1_REGISTER_8BIT Al;
	SOL1_REGISTER_8BIT Bh; // BX (16bit) Base		(Bh/Bl)
	SOL1_REGISTER_8BIT Bl;
	SOL1_REGISTER_8BIT Ch; // CX (16bit) Counter		(Ch/Cl)
	SOL1_REGISTER_8BIT Cl;
	SOL1_REGISTER_8BIT Dh; // DX (16bit) Data		(Dh/Dl)
	SOL1_REGISTER_8BIT Dl;
	SOL1_REGISTER_8BIT Gh; // GX (16bit)	Gh/Gl	General Register(For scratch)
	SOL1_REGISTER_8BIT Gl;

	//Pointer Registers
	SOL1_REGISTER_8BIT BPh; // BP (16bit) Base Pointer  (Used to manage stack frames)
	SOL1_REGISTER_8BIT BPl;
	SOL1_REGISTER_8BIT SPh; // SP (16bit) Stack Pointer
	SOL1_REGISTER_8BIT SPl;
	SOL1_REGISTER_8BIT SSPh; // SSP (16bit) Supervisor Stack Pointer
	SOL1_REGISTER_8BIT SSPl;

	//Index Registers
	SOL1_REGISTER_8BIT SIh; // SI (16bit) Source index (Source address for string operations)
	SOL1_REGISTER_8BIT SIl;
	SOL1_REGISTER_8BIT DIh; // DI (16bit) Destination Index (Destination address for string operations)
	SOL1_REGISTER_8BIT DIl;
	SOL1_REGISTER_8BIT PCh; // PC (16bit) Program Counter
	SOL1_REGISTER_8BIT PCl;

	SOL1_REGISTER_8BIT TDRh; // TDR (16bit) Temporary Data Register
	SOL1_REGISTER_8BIT TDRl;
	SOL1_REGISTER_8BIT PTB;  // PTB (8bit) = Page table base



	SOL1_REGISTER_8BIT MSWh; // MSW (16bit) FLAGS
	SOL1_REGISTER_8BIT MSWl; // STATUS - flags de controle


	SOL1_REGISTER_8BIT INT_MASKS; // INT FLAGS
	


	SOL1_REGISTER_8BIT MARh; // memory address register
	SOL1_REGISTER_8BIT MARl;
	SOL1_REGISTER_8BIT MDRh; // memory data register
	SOL1_REGISTER_8BIT MDRl;

	//Control Registers (FLAGS)
		//ZF // Zero Flag
		//CF // Carry Flag
		//SF // Sign Flag
		//OF // Overflow Flag

	//////////////////////////////////////////////////
    //unsigned char S;       /*flag S     Result negative         */
    //unsigned char Z;       /*flag Z     Result is zero          */
    //unsigned char H;       /*flag H     Halfcarry/Halfborrow    */
    //unsigned char P;       /*flag P     Result is even          */
    //unsigned char V;       /*flag V     Overflow occured        */
    //unsigned char N;       /*flag N     Subtraction occured     */
    //unsigned char C;       /*flag C     Carry/Borrow occured    */


	SOL1_REGISTERS();

	static SOL1_DWORD value(SOL1_REGISTER_8BIT& l, SOL1_REGISTER_8BIT& h);
	static void set(SOL1_REGISTER_8BIT& l, SOL1_REGISTER_8BIT& h, SOL1_DWORD v);
	static void reset(SOL1_REGISTER_8BIT& l, SOL1_REGISTER_8BIT& h);
};






#endif