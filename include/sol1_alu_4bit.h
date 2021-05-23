#ifndef SOL1ALU4BIT_H
#define SOL1ALU4BIT_H

#include "config.h"

struct sol1_alu_4bit
{
	SOL1_BYTE _A;
	SOL1_BYTE _B;
	SOL1_BYTE _C;

	SOL1_BYTE A;
	SOL1_BYTE B;

	SOL1_BYTE C;
	SOL1_BYTE alu_output;

	SOL1_BYTE CIN; //carry in

	SOL1_BYTE COUT; //carry out

	SOL1_BYTE EQ; //EQ
	SOL1_BYTE P;
	SOL1_BYTE G;

	SOL1_BYTE alu_zf; // flags do alu
	SOL1_BYTE alu_cf;
	SOL1_BYTE alu_of;

	SOL1_BYTE alu_final_cf;

	SOL1_BYTE alu_op;
	SOL1_BYTE alu_mode;
	SOL1_BYTE alu_cf_in_src;
	SOL1_BYTE alu_cf_in_inv;
	SOL1_BYTE alu_cf_out_inv;

	SOL1_BYTE alu_a_src;
	SOL1_BYTE alu_b_src;
};

void sol1_alu_4bit_xor(struct sol1_alu_4bit* alu);
void sol1_alu_4bit_or(struct sol1_alu_4bit* alu);
void sol1_alu_4bit_and(struct sol1_alu_4bit* alu);
void sol1_alu_4bit_not(struct sol1_alu_4bit* alu);
void sol1_alu_4bit_shl(struct sol1_alu_4bit* alu);
void sol1_alu_4bit_shr(struct sol1_alu_4bit* alu);

void sol1_alu_4bit_sum(struct sol1_alu_4bit* alu);
void sol1_alu_4bit_sub(struct sol1_alu_4bit* alu);

SOL1_BYTE sol1_alu_4bit_op(struct sol1_alu_4bit* alu, SOL1_BYTE A, SOL1_BYTE B, SOL1_BYTE CIN, SOL1_BYTE S, SOL1_BYTE M);


void sol1_alu_4bit_init(struct sol1_alu_4bit *alu);

#endif