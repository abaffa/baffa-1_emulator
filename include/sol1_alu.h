#ifndef SOL1ALU_H
#define SOL1ALU_H

#include "config.h"

#define SOL1_ALU_ZERO		0x01
#define SOL1_ALU_EQUAL		0x02
#define SOL1_ALU_LARGER		0x04
#define SOL1_ALU_CARRY_OUT	0x08

struct sol1_alu
{
	SOL1_DWORD A;
	SOL1_DWORD B;

	SOL1_DWORD C;

	SOL1_BYTE CIN; //carry in
	SOL1_BYTE F; //Larger, equal, zero, carry out


	SOL1_BYTE OP; //
	

};

void sol1_alu_xor(struct sol1_alu* alu);
void sol1_alu_or(struct sol1_alu* alu);
void sol1_alu_and(struct sol1_alu* alu);
void sol1_alu_not(struct sol1_alu* alu);
void sol1_alu_shl(struct sol1_alu* alu);
void sol1_alu_shr(struct sol1_alu* alu);

void sol1_alu_sum(struct sol1_alu* alu);
void sol1_alu_sub(struct sol1_alu* alu);


#endif