#ifndef SOL1ALUBASIC_H
#define SOL1ALUBASIC_H

#include "config.h"

#define SOL1_ALU_ZERO		0x01
#define SOL1_ALU_EQUAL		0x02
#define SOL1_ALU_LARGER		0x04
#define SOL1_ALU_CARRY_OUT	0x08

struct sol1_alu_basic
{
	SOL1_DWORD A;
	SOL1_DWORD B;

	SOL1_DWORD C;

	SOL1_BYTE CIN; //carry in
	SOL1_BYTE F; //Larger, equal, zero, carry out


	SOL1_BYTE OP; //


};

void sol1_alu_basic_xor(struct sol1_alu_basic* alu);
void sol1_alu_basic_or(struct sol1_alu_basic* alu);
void sol1_alu_basic_and(struct sol1_alu_basic* alu);
void sol1_alu_basic_not(struct sol1_alu_basic* alu);
void sol1_alu_basic_shl(struct sol1_alu_basic* alu);
void sol1_alu_basic_shr(struct sol1_alu_basic* alu);

void sol1_alu_basic_sum(struct sol1_alu_basic* alu);
void sol1_alu_basic_sub(struct sol1_alu_basic* alu);


#endif