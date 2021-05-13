#ifndef SOL1REGISTERS_H
#define SOL1REGISTERS_H

#include "config.h"
struct sol1_registers
{
    //General Purpose Registers
	//DATA REGISTERS
	SOL1_DWORD A; // AX (16bit) Accumulator	(Ah/Al)
	SOL1_DWORD B; // BX (16bit) Base		(Bh/Bl)
	SOL1_DWORD C; // CX (16bit) Counter		(Ch/Cl)
	SOL1_DWORD D; // DX (16bit) Data		(Dh/Dl)
	SOL1_DWORD G; // GX (16bit)	Gh/Gl	General Register(For scratch)

	//Pointer Registers
	SOL1_MWORD BP; // BP (16bit) Base Pointer  (Used to manage stack frames)
	SOL1_MWORD SP; // SP (16bit) Stack Pointer

	//Index Registers
	SOL1_MWORD SI; // SI (16bit) Source index (Source address for string operations)
	SOL1_MWORD DI; // DI (16bit) Destination Index (Destination address for string operations)
	SOL1_MWORD PC; // PC (16bit) Program Counter

	SOL1_MWORD TDR; // TDR (16bit) Temporary Data Register
	SOL1_MWORD SSP; // SSP (16bit) Supervisor Stack Pointer
	SOL1_BYTE PTB;  // PTB (8bit) = Page table base



    //Status register(F) - flags

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
};


#endif