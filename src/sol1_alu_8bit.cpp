#include "sol1_alu_8bit.h"
#include "utils.h"


static void sol1_alu_reset(struct sol1_alu_8bit *alu) {

	alu->_A = 0x00;
	alu->_B = 0x00;
	alu->_C = 0x00;

	alu->A = 0x00;
	alu->B = 0x00;

	alu->C = 0x00;
	alu->alu_output = 0x00;

	alu->CIN = 0x00; //carry in

	alu->COUT = 0x00; //carry in

	alu->EQ = 0x00; //carry in
	alu->F = 0x00; //Larger, equal, zero, carry out

}
void sol1_alu_init(struct sol1_alu_8bit *alu) {

	sol1_alu_reset(alu);

	// flags do alu
	alu->alu_zf = 0x00; // ZeroFlag
	alu->alu_cf = 0x00; // Carry Flag
	alu->alu_of = 0x00; // Overflow Flag

	alu->alu_final_cf = 0x00;

	alu->alu_op = 0x00;
	alu->alu_mode = 0x00;
	alu->alu_cf_in_src = 0x00;  // Carry Flag
	alu->alu_cf_in_inv = 0x00;  // Carry Flag
	alu->alu_cf_out_inv = 0x00;  // Carry Flag

	alu->alu_a_src = 0x00;
	alu->alu_b_src = 0x00;
}

void sol1_alu_8bit_alu_chk_zero(struct sol1_alu_8bit* alu) {
	if (alu->C == 0)  //zero
		alu->F = alu->F | SOL1_ALU_ZERO;
	else
		alu->F = alu->F & ~SOL1_ALU_ZERO;
}

void sol1_alu_8bit_alu_chk_eq(struct sol1_alu_8bit* alu)
{
	if (alu->A == alu->B) {  //equal
		alu->F = alu->F | SOL1_ALU_EQUAL;
		alu->EQ = 0x01;
	}
	else
		alu->F = alu->F & ~SOL1_ALU_EQUAL;
}

void sol1_alu_8bit_alu_chk_lager(struct sol1_alu_8bit* alu)
{
	if (alu->A < alu->B)  //larger
		alu->F = alu->F | SOL1_ALU_LARGER;
	else
		alu->F = alu->F & ~SOL1_ALU_LARGER;
}


/////////////////////////////////////////////


void sol1_alu_8bit_xor(struct sol1_alu_8bit* alu) {

	alu->C = (alu->A ^ alu->B);

	sol1_alu_8bit_alu_chk_lager(alu);

	sol1_alu_8bit_alu_chk_eq(alu);

	sol1_alu_8bit_alu_chk_zero(alu);
}

void sol1_alu_8bit_or(struct sol1_alu_8bit* alu) {
	alu->C = (alu->A | alu->B);

	sol1_alu_8bit_alu_chk_zero(alu);
}

void sol1_alu_8bit_and(struct sol1_alu_8bit* alu) {
	alu->C = (alu->A & alu->B);

	sol1_alu_8bit_alu_chk_zero(alu);
}

void sol1_alu_8bit_not(struct sol1_alu_8bit* alu) {
	alu->C = (~alu->A);

	sol1_alu_8bit_alu_chk_zero(alu);
}

void sol1_alu_8bit_shl(struct sol1_alu_8bit* alu) {
	alu->C = ((alu->A << 1) | (alu->CIN & 0x01));

	if ((((alu->A >> 8) - 1) & 0x01) == 0x01) { //shift out..
		alu->F = alu->F | SOL1_ALU_CARRY_OUT;
		alu->COUT = 0x01;
	}
	else
		alu->F = alu->F & ~SOL1_ALU_CARRY_OUT;

	sol1_alu_8bit_alu_chk_zero(alu);
	alu->CIN = 0;
}

void sol1_alu_8bit_shr(struct sol1_alu_8bit* alu) {
	alu->C = ((alu->A >> 1) | ((0xFF >> 1) | (alu->CIN << 7)));

	if ((alu->A & 0x01) == 0x01) { //shift out..
		alu->F = alu->F | SOL1_ALU_CARRY_OUT;
		alu->COUT = 0x01;
	}
	else
		alu->F = alu->F & ~SOL1_ALU_CARRY_OUT;

	sol1_alu_8bit_alu_chk_zero(alu);
	alu->CIN = 0;
}


void sol1_alu_8bit_opsum(struct sol1_alu_8bit* alu, SOL1_BYTE B) {

	alu->C = (alu->A + B + (alu->CIN & 0x01)); //Carry in

	if (B > (0xFF - (alu->A + (alu->CIN & 0x01)))) { //carry out
		alu->F = alu->F | SOL1_ALU_CARRY_OUT;
		alu->COUT = 0x01;
	}
	else
		alu->F = alu->F & ~SOL1_ALU_CARRY_OUT;

	sol1_alu_8bit_alu_chk_zero(alu);
	alu->CIN = 0;
}
void sol1_alu_8bit_sum(struct sol1_alu_8bit* alu) {
	sol1_alu_8bit_opsum(alu, alu->B);
}

void sol1_alu_8bit_opsub(struct sol1_alu_8bit* alu, SOL1_BYTE B) {
	//Carry in
	//alu->C = (alu->A - (B + (alu->CIN & 0x01))); //Carry in 
	alu->C = ((alu->A - B) + (alu->CIN & 0x01)); //Carry in ( ajuste para 74LS181

	if ((B + (alu->CIN & 0x01)) > alu->A) { //carry out
		alu->F = alu->F | SOL1_ALU_CARRY_OUT;
		alu->COUT = 0x01;
	}
	else
		alu->F = alu->F & ~SOL1_ALU_CARRY_OUT;

	sol1_alu_8bit_alu_chk_zero(alu);
	alu->CIN = 0;
}

void sol1_alu_8bit_sub(struct sol1_alu_8bit* alu) {
	sol1_alu_8bit_opsub(alu, alu->B);
}






static SOL1_BYTE sol1_alu_inout_low(struct sol1_alu_8bit* alu, SOL1_BYTE A, SOL1_BYTE B, SOL1_BYTE S, SOL1_BYTE M) {

	alu->A = A;
	alu->B = B;
	alu->C = 0x00;
	alu->alu_output = 0x00;
	//alu->CIN = CIN & 0b00000001;
	alu->COUT = 0x00;
	alu->EQ = 0x00;
	alu->F = 0x00; //Larger, equal, zero, carry out

	alu->alu_zf = 0x00; // ZeroFlag
	alu->alu_cf = 0x00; // Carry Flag
	alu->alu_of = 0x00; // Overflow Flag

	alu->alu_final_cf = 0x00;

	switch (S) {
	case 0x0:
		if (M != 0x00) {
			sol1_alu_8bit_not(alu);
		}
		else {
			sol1_alu_8bit_opsub(alu, 0x01);
		}
		break;
	case 0x1:
		if (M != 0x00) {
			sol1_alu_8bit_and(alu);
			alu->C = (~alu->C);
			sol1_alu_8bit_alu_chk_zero(alu);
		}
		else {
			sol1_alu_8bit_and(alu);
			alu->A = alu->C;
			sol1_alu_8bit_opsub(alu, 0x01);
		}
		break;
	case 0x2:
		if (M != 0x00) {
			alu->A = ~A;
			sol1_alu_8bit_or(alu);
		}
		else {
			alu->A = A;
			alu->B = (~B);
			sol1_alu_8bit_and(alu);
			alu->A = alu->C;
			sol1_alu_8bit_opsub(alu, 0x01);
		}
		break;
	case 0x3:
		if (M != 0x00) {
			alu->C = 0xF;
		}
		else {
			alu->A = 0x0;
			sol1_alu_8bit_opsub(alu, 0x01);
		}
		break;
	case 0x4:
		if (M != 0x00) {
			sol1_alu_8bit_or(alu);
			alu->C = (~alu->C);
			sol1_alu_8bit_alu_chk_zero(alu);
		}
		else {
			alu->B = (~B);
			sol1_alu_8bit_or(alu);
			SOL1_BYTE AorNB = alu->C;

			alu->B = AorNB;
			sol1_alu_8bit_sum(alu);
		}
		break;
	case 0x5:
		if (M != 0x00) {
			alu->C = (~alu->B);
			sol1_alu_8bit_alu_chk_zero(alu);
		}
		else {

			sol1_alu_8bit_and(alu);
			SOL1_BYTE AandB = alu->C;

			alu->B = (~B);
			sol1_alu_8bit_or(alu);
			SOL1_BYTE AorNB = alu->C;

			alu->A = AandB;
			alu->B = AorNB;
			sol1_alu_8bit_sum(alu);
		}
		break;
	case 0x6:
		if (M != 0x00) {
			sol1_alu_8bit_xor(alu);
			alu->C = (~alu->C);
			sol1_alu_8bit_alu_chk_zero(alu);
		}
		else {
			sol1_alu_8bit_sub(alu);
			alu->A = alu->C;
			sol1_alu_8bit_opsub(alu, 0x01);
		}
		break;
	case 0x7:
		if (M != 0x00) {
			alu->A = A;
			alu->B = (~B);
			sol1_alu_8bit_or(alu);
		}
		else {
			alu->A = A;
			alu->B = (~B);
			sol1_alu_8bit_or(alu);
		}
		break;
	case 0x8:
		if (M != 0x00) {
			alu->A = (~A);
			alu->B = B;
			sol1_alu_8bit_and(alu);
		}
		else {
			sol1_alu_8bit_or(alu);

			alu->B = alu->C;
			sol1_alu_8bit_sum(alu);
		}
		break;
	case 0x9:
		if (M != 0x00) {
			alu->A = A;
			alu->B = B;
			sol1_alu_8bit_xor(alu);
		}
		else {
			sol1_alu_8bit_sum(alu);
		}
		break;
	case 0xA:
		if (M != 0x00) {
			alu->C = B;
			sol1_alu_8bit_alu_chk_zero(alu);
		}
		else {

			alu->B = (~B);
			sol1_alu_8bit_and(alu);
			SOL1_BYTE AandNB = alu->C;

			alu->B = B;
			sol1_alu_8bit_or(alu);
			SOL1_BYTE AorB = alu->C;

			alu->A = AandNB;
			alu->B = AorB;
			sol1_alu_8bit_sum(alu);
		}
		break;
	case 0xB:
		if (M != 0x00) {
			sol1_alu_8bit_or(alu);
		}
		else
		{
			sol1_alu_8bit_or(alu);
		}
		break;
	case 0xC:
		if (M != 0x00) {
			alu->C = 0x00;
			sol1_alu_8bit_alu_chk_zero(alu);
		}
		else {
			sol1_alu_8bit_opsum(alu, alu->A);
		}
		break;
	case 0xD:
		if (M != 0x00) {
			alu->A = A;
			alu->B = (~B);
			sol1_alu_8bit_and(alu);
		}
		else {
			sol1_alu_8bit_and(alu);
			SOL1_BYTE AandB = alu->C;
			alu->A = AandB;
			alu->B = A;

			sol1_alu_8bit_sum(alu);
		}
		break;
	case 0xE:
		if (M != 0x00) {
			sol1_alu_8bit_and(alu);
		}
		else {
			alu->B = (~B);
			sol1_alu_8bit_and(alu);
			SOL1_BYTE AandNB = alu->C;
			alu->A = AandNB;
			alu->B = A;

			sol1_alu_8bit_sum(alu);
		}
		break;
	case 0xF:

		if (M != 0x00) {
			alu->C = A;
			sol1_alu_8bit_alu_chk_zero(alu);
		}
		else {
			sol1_alu_8bit_opsum(alu, 0x00);
		}

		break;
	}

	sol1_alu_8bit_alu_chk_zero(alu);
	sol1_alu_8bit_alu_chk_eq(alu);
	sol1_alu_8bit_alu_chk_lager(alu);

	alu->alu_output = alu->C;
	return alu->alu_output;
}



static SOL1_BYTE sol1_alu_inout_high(struct sol1_alu_8bit* alu, SOL1_BYTE A, SOL1_BYTE B, SOL1_BYTE S, SOL1_BYTE M) {

	alu->A = A;
	alu->B = B;
	alu->C = 0x00;
	alu->alu_output = 0x00;
	//alu->CIN = CIN & 0b00000001;
	alu->COUT = 0x00;
	alu->EQ = 0x00;
	alu->F = 0x00; //Larger, equal, zero, carry out

	alu->alu_zf = 0x00; // ZeroFlag
	alu->alu_cf = 0x00; // Carry Flag
	alu->alu_of = 0x00; // Overflow Flag

	alu->alu_final_cf = 0x00;

	switch (S) {
	case 0x0:
		if (M != 0x00) {
			sol1_alu_8bit_not(alu);
		}
		else {
			sol1_alu_8bit_opsum(alu, 0x00);
		}
		break;
	case 0x1:
		if (M != 0x00) {

			sol1_alu_8bit_or(alu);
			alu->C = (~alu->C);
			sol1_alu_8bit_alu_chk_zero(alu);
		}
		else {
			sol1_alu_8bit_and(alu);
		}
		break;
	case 0x2:
		if (M != 0x00) {
			alu->A = ~A;
			alu->B = B;
			sol1_alu_8bit_and(alu);
		}
		else {
			alu->A = A;
			alu->B = ~B;
			sol1_alu_8bit_or(alu);
		}
		break;
	case 0x3:
		if (M != 0x00) {
			alu->C = 0x00;
		}
		else {
			alu->A = 0x0;
			sol1_alu_8bit_opsub(alu, 0x01);
		}
		break;
	case 0x4:
		if (M != 0x00) {
			sol1_alu_8bit_and(alu);
			alu->C = (~alu->C);
			sol1_alu_8bit_alu_chk_zero(alu);
		}
		else {
			alu->B = (~B);
			sol1_alu_8bit_and(alu);

			alu->B = alu->C;
			sol1_alu_8bit_sum(alu);
		}
		break;
	case 0x5:
		if (M != 0x00) {
			alu->C = (~alu->B);
			sol1_alu_8bit_alu_chk_zero(alu);
		}
		else {

			sol1_alu_8bit_or(alu);
			SOL1_BYTE AorB = alu->C;

			alu->B = (~B);
			sol1_alu_8bit_and(alu);
			SOL1_BYTE AandNB = alu->C;

			alu->A = AorB;
			alu->B = AandNB;
			sol1_alu_8bit_sum(alu);
		}
		break;
	case 0x6:
		if (M != 0x00) {
			sol1_alu_8bit_xor(alu);
			sol1_alu_8bit_alu_chk_zero(alu);
		}
		else {
			sol1_alu_8bit_sub(alu);
		}
		break;
	case 0x7:
		if (M != 0x00) {
			alu->A = A;
			alu->B = (~B);
			sol1_alu_8bit_and(alu);
		}
		else {
			alu->B = (~B);
			sol1_alu_8bit_and(alu);
			alu->A = alu->C;
			sol1_alu_8bit_opsub(alu, 0x01);
		}
		break;
	case 0x8:
		if (M != 0x00) {
			alu->A = (~A);
			sol1_alu_8bit_or(alu);
		}
		else {
			sol1_alu_8bit_and(alu);
			alu->B = alu->C;
			sol1_alu_8bit_sum(alu);
		}
		break;
	case 0x9:
		if (M != 0x00) {
			alu->A = (~A);
			alu->B = (~B);
			sol1_alu_8bit_xor(alu);
		}
		else {
			sol1_alu_8bit_sum(alu);

			alu->A = alu->C;
			alu->B = 1;
			sol1_alu_8bit_sum(alu);
		}
		break;
	case 0xA:
		if (M != 0x00) {
			alu->C = B;
			sol1_alu_8bit_alu_chk_zero(alu);
		}
		else {
			alu->B = (~B);
			sol1_alu_8bit_or(alu);
			SOL1_BYTE AorNB = alu->C;

			alu->A = A;
			alu->B = B;
			sol1_alu_8bit_and(alu);
			SOL1_BYTE AandB = alu->C;

			alu->A = AorNB;
			alu->B = AandB;
			sol1_alu_8bit_sum(alu);
		}
		break;
	case 0xB:
		if (M != 0x00) {
			sol1_alu_8bit_and(alu);
		}
		else {
			sol1_alu_8bit_and(alu);
			alu->A = alu->C;
			sol1_alu_8bit_opsub(alu, 0x01);
		}
		break;
	case 0xC:
		if (M != 0x00) {
			alu->C = 0xFFFF;
		}
		else {
			sol1_alu_8bit_opsum(alu, alu->A);
		}
		break;
	case 0xD:
		if (M != 0x00) {
			alu->A = A;
			alu->B = (~B);
			sol1_alu_8bit_or(alu);
		}
		else {
			sol1_alu_8bit_or(alu);
			SOL1_BYTE AorB = alu->C;
			alu->B = alu->A;
			alu->A = AorB;

			sol1_alu_8bit_sum(alu);
		}
		break;
	case 0xE:
		if (M != 0x00) {
			sol1_alu_8bit_or(alu);
		}
		else {
			alu->B = (~B);
			sol1_alu_8bit_or(alu);
			SOL1_BYTE AorNB = alu->C;

			alu->B = alu->A;
			alu->A = AorNB;

			sol1_alu_8bit_sum(alu);
		}
		break;
	case 0xF:

		if (M != 0x00) {
			alu->C = A;
			sol1_alu_8bit_alu_chk_zero(alu);
		}
		else {
			sol1_alu_8bit_opsub(alu, 0x01);
		}

		break;
	}


	sol1_alu_8bit_alu_chk_zero(alu);
	sol1_alu_8bit_alu_chk_eq(alu);
	sol1_alu_8bit_alu_chk_lager(alu);

	alu->alu_output = alu->C;
	return alu->alu_output;
}





//74LS181
SOL1_BYTE sol1_alu_8bit_op(struct sol1_alu_8bit* alu, SOL1_BYTE A, SOL1_BYTE B, SOL1_BYTE CIN, SOL1_BYTE S, SOL1_BYTE M) {

	sol1_alu_reset(alu);

	alu->_A = A;
	alu->_B = B;

	/*
	if (M != 0x00)
		//alu->alu_output = sol1_alu_inout_low(alu, A, B, S, M); // default
		alu->alu_output = sol1_alu_inout_high(alu, A, B, S, M);  // WEIRD!
	else {
		if (CIN == 0x00)
			alu->alu_output = sol1_alu_inout_low(alu, A, B, S, M);
		else
			alu->alu_output = sol1_alu_inout_high(alu, A, B, S, M);
	}
	*/
	if (CIN == 0x00)
		alu->alu_output = sol1_alu_inout_low(alu, A, B, S, M);
	else
		alu->alu_output = sol1_alu_inout_high(alu, A, B, S, M);

	alu->_C = alu->alu_output;

	return alu->alu_output;
}


void sol1_alu_display_registers(struct sol1_alu_8bit *alu) {

	printf("* A="); print_byte_bin(alu->_A); printf(" | ");
	printf("B="); print_byte_bin(alu->_B); printf(" | ");
	printf("C="); print_byte_bin(alu->_C); printf(" | ");
	printf("Cin="); print_nibble_bin(alu->CIN); printf(" | ");
	printf("Cout="); print_nibble_bin(alu->COUT);
	printf("\n");

	printf("* alu_op: "); print_nibble_bin(alu->alu_op); printf(" | alu_mode: "); print_nibble_bin(alu->alu_mode); printf("\n");
	printf("* alu_output="); print_byte_bin(alu->alu_output); printf("\n");
	printf("* EQ="); print_nibble_bin(alu->EQ); printf(" | ");	printf("F="); print_byte_bin(alu->F); printf("\n");
	printf("* Flags: ");

	if (alu->alu_zf != 0x00) printf("alu_zf ");
	if (alu->alu_cf != 0x00) printf("alu_cf ");
	if (alu->alu_of != 0x00) printf("alu_of ");
	if (alu->alu_final_cf != 0x00) printf("alu_final_cf ");
	printf("\n");
	printf("* Microcode: ");
	if (alu->alu_cf_in_src != 0x00) printf("alu_cf_in_src ");
	if (alu->alu_cf_in_inv != 0x00) printf("alu_cf_in_inv ");
	if (alu->alu_cf_out_inv != 0x00) printf("alu_cf_out_inv ");

	if (alu->alu_a_src != 0x00) printf("alu_a_src ");
	if (alu->alu_b_src != 0x00) printf("alu_b_src ");
	printf("\n");
}
