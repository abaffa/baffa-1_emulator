#ifndef SOL1ALU8BIT_H
#define SOL1ALU8BIT_H

#include "config.h"

#define SOL1_ALU_ZERO		0x01
#define SOL1_ALU_EQUAL		0x02
#define SOL1_ALU_LARGER		0x04
#define SOL1_ALU_CARRY_OUT	0x08
#define SOL1_ALU_CARRY_G_OUT	0x16 // Carry Generate Output
#define SOL1_ALU_CARRY_P_OUT	0x32 // Carry Propagate Output

struct sol1_alu_8bit
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
	SOL1_BYTE F; //Larger, equal, zero, carry out


	//SOL1_BYTE OP; //
	


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

void sol1_alu_8bit_xor(struct sol1_alu_8bit* alu);
void sol1_alu_8bit_or(struct sol1_alu_8bit* alu);
void sol1_alu_8bit_and(struct sol1_alu_8bit* alu);
void sol1_alu_8bit_not(struct sol1_alu_8bit* alu);
void sol1_alu_8bit_shl(struct sol1_alu_8bit* alu);
void sol1_alu_8bit_shr(struct sol1_alu_8bit* alu);

void sol1_alu_8bit_sum(struct sol1_alu_8bit* alu);
void sol1_alu_8bit_sub(struct sol1_alu_8bit* alu);

SOL1_BYTE sol1_alu_8bit_op(struct sol1_alu_8bit* alu, SOL1_BYTE A, SOL1_BYTE B, SOL1_BYTE CIN, SOL1_BYTE S, SOL1_BYTE M);


void sol1_alu_init(struct sol1_alu_8bit *alu);
void sol1_alu_display_registers(struct sol1_alu_8bit *alu);

#endif