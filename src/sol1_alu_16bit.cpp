#include "sol1_alu_16bit.h"


void sol1_alu_16bit_xor(struct sol1_alu_16bit* alu){	
	
	alu->C = alu->A ^ alu->B;
	
	if (alu->A < alu->B)  //larger
		alu->F = alu->F | SOL1_ALU_LARGER;
	else
		alu->F = alu->F & ~SOL1_ALU_LARGER;
	
	if (alu->A == alu->B)  //equal
		alu->F = alu->F | SOL1_ALU_EQUAL;
	else
		alu->F = alu->F & ~SOL1_ALU_EQUAL;
	
	if(alu->C == 0)  //zero
		alu->F = alu->F | SOL1_ALU_ZERO;
	else
		alu->F = alu->F & ~SOL1_ALU_ZERO;
}

void sol1_alu_16bit_or(struct sol1_alu_16bit* alu){
	alu->C = alu->A | alu->B;
	
	if (alu->C == 0)  //zero
		alu->F = alu->F | SOL1_ALU_ZERO;
	else
		alu->F = alu->F & ~SOL1_ALU_ZERO;
}

void sol1_alu_16bit_and(struct sol1_alu_16bit* alu){
	alu->C = alu->A & alu->B;
	
	if (alu->C == 0)  //zero
		alu->F = alu->F | SOL1_ALU_ZERO;
	else
		alu->F = alu->F & ~SOL1_ALU_ZERO;
}

void sol1_alu_16bit_not(struct sol1_alu_16bit* alu) {
	alu->C = ~alu->A;
	
	if (alu->C == 0)  //zero
		alu->F = alu->F | SOL1_ALU_ZERO;
	else
		alu->F = alu->F & ~SOL1_ALU_ZERO;
}

void sol1_alu_16bit_shl(struct sol1_alu_16bit* alu) {
	alu->C = (alu->A << 1) | (alu->CIN & 0x01);

	if(((alu->A >> 16 - 1) & 0x01) == 0x01) //shift out..
		alu->F = alu->F | SOL1_ALU_CARRY_OUT;
	else
		alu->F = alu->F & ~SOL1_ALU_CARRY_OUT;
	
	if (alu->C == 0)  //zero
		alu->F = alu->F | SOL1_ALU_ZERO;
	else
		alu->F = alu->F & ~SOL1_ALU_ZERO;
}

void sol1_alu_16bit_shr(struct sol1_alu_16bit* alu) {
	alu->C = (alu->A >> 1) | ((0xFFFF >> 1) | (alu->CIN << 15));

	if ((alu->A & 0x01) == 0x01) //shift out..
		alu->F = alu->F | SOL1_ALU_CARRY_OUT;
	else
		alu->F = alu->F & ~SOL1_ALU_CARRY_OUT;

	if (alu->C == 0)  //zero
		alu->F = alu->F | SOL1_ALU_ZERO;
	else
		alu->F = alu->F & ~SOL1_ALU_ZERO;
}

void sol1_alu_16bit_sum(struct sol1_alu_16bit* alu) {
	
	alu->C = alu->A + alu->B + (alu->CIN & 0x01); //Carry in
		
	if (alu->B > (0xFFFF - (alu->A + (alu->CIN & 0x01)))) //carry out
		alu->F = alu->F | SOL1_ALU_CARRY_OUT;
	else
		alu->F = alu->F & ~SOL1_ALU_CARRY_OUT;
	
	if (alu->C == 0)  //zero
		alu->F = alu->F | SOL1_ALU_ZERO;
	else
		alu->F = alu->F & ~SOL1_ALU_ZERO;
}

void sol1_alu_16bit_sub(struct sol1_alu_16bit* alu) {
	//Carry in
	alu->C = alu->A - (alu->B + (alu->CIN & 0x01)); //Carry in

	if ((alu->B + (alu->CIN & 0x01)) > alu->A) //carry out
		alu->F = alu->F | SOL1_ALU_CARRY_OUT;
	else
		alu->F = alu->F & ~SOL1_ALU_CARRY_OUT;
	
	if (alu->C == 0)  //zero
		alu->F = alu->F | SOL1_ALU_ZERO;
	else
		alu->F = alu->F & ~SOL1_ALU_ZERO;
}