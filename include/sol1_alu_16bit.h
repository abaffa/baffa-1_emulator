#ifndef SOL1ALU16BIT_H
#define SOL1ALU16BIT_H

#include "config.h"

#define SOL1_ALU_ZERO		0x01
#define SOL1_ALU_EQUAL		0x02
#define SOL1_ALU_LARGER		0x04
#define SOL1_ALU_CARRY_OUT	0x08

struct sol1_alu_16bit
{
	SOL1_DWORD A;
	SOL1_DWORD B;

	SOL1_DWORD C;

	SOL1_BYTE CIN; //carry in
	SOL1_BYTE F; //Larger, equal, zero, carry out


	SOL1_BYTE OP; //
	

};

void sol1_alu_16bit_xor(struct sol1_alu_16bit* alu);
void sol1_alu_16bit_or(struct sol1_alu_16bit* alu);
void sol1_alu_16bit_and(struct sol1_alu_16bit* alu);
void sol1_alu_16bit_not(struct sol1_alu_16bit* alu);
void sol1_alu_16bit_shl(struct sol1_alu_16bit* alu);
void sol1_alu_16bit_shr(struct sol1_alu_16bit* alu);

void sol1_alu_16bit_sum(struct sol1_alu_16bit* alu);
void sol1_alu_16bit_sub(struct sol1_alu_16bit* alu);


#endif