#ifndef SOL1ALU_H
#define SOL1ALU_H

#include "sol1_alu_4bit.h"
#include "config.h"



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


void sol1_alu_init(struct sol1_alu_8bit *alu);
void sol1_alu_display_registers(struct sol1_alu_8bit *alu);


SOL1_BYTE ALU_EXEC(sol1_alu_8bit *alu, SOL1_BYTE x_bus, SOL1_BYTE y_bus, 
	SOL1_BYTE u_cf, SOL1_BYTE msw_cf, SOL1_BYTE shift_src, SOL1_BYTE zbus_out_src, SOL1_BYTE DEBUG_ALU);


#endif