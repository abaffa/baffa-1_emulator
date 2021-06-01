#include "sol1_alu_4bit.h"
#include "utils.h"

// INVERTER
static SOL1_BYTE IC_74LS04(SOL1_BYTE A) {
	return get_byte_bit(~A, 0);
}

static SOL1_BYTE IC74LS181_B1(SOL1_BYTE nA, SOL1_BYTE nB, SOL1_BYTE S, int block) {

	SOL1_BYTE S0 = get_byte_bit(S, 0);
	SOL1_BYTE S1 = get_byte_bit(S, 1);

	SOL1_BYTE andA = get_byte_bit(nA, block);
	SOL1_BYTE andB = get_byte_bit(nB, block) & S0;
	SOL1_BYTE andC = S1 & IC_74LS04(get_byte_bit(nB, block));

	return IC_74LS04(andA | andB | andC);
}

static SOL1_BYTE IC74LS181_B2(SOL1_BYTE nA, SOL1_BYTE nB, SOL1_BYTE S, int block) {

	SOL1_BYTE S2 = get_byte_bit(S, 2);
	SOL1_BYTE S3 = get_byte_bit(S, 3);

	SOL1_BYTE andA = IC_74LS04(get_byte_bit(nB, block)) & S2 & get_byte_bit(nA, block);
	SOL1_BYTE andB = get_byte_bit(nA, block) & get_byte_bit(nB, block) & S3;

	return IC_74LS04(andA | andB);

}

static SOL1_BYTE IC74LSL181(SOL1_BYTE nA, SOL1_BYTE nB, SOL1_BYTE S, SOL1_BYTE M, SOL1_BYTE Cn,
	SOL1_BYTE *AeqB, SOL1_BYTE *Cn4, SOL1_BYTE *nP, SOL1_BYTE *nG) {

	SOL1_BYTE B0_1 = IC74LS181_B1(nA, nB, S, 0);
	SOL1_BYTE nB0_1 = IC_74LS04(B0_1);
	SOL1_BYTE B0_2 = IC74LS181_B2(nA, nB, S, 0);

	SOL1_BYTE B1_1 = IC74LS181_B1(nA, nB, S, 1);
	SOL1_BYTE nB1_1 = IC_74LS04(B1_1);
	SOL1_BYTE B1_2 = IC74LS181_B2(nA, nB, S, 1);

	SOL1_BYTE B2_1 = IC74LS181_B1(nA, nB, S, 2);
	SOL1_BYTE nB2_1 = IC_74LS04(B2_1);
	SOL1_BYTE B2_2 = IC74LS181_B2(nA, nB, S, 2);

	SOL1_BYTE B3_1 = IC74LS181_B1(nA, nB, S, 3);
	SOL1_BYTE nB3_1 = IC_74LS04(B3_1);
	SOL1_BYTE B3_2 = IC74LS181_B2(nA, nB, S, 3);

	SOL1_BYTE nM = IC_74LS04(M);

	SOL1_BYTE nF0 = IC_74LS04(Cn & nM) ^ (nB0_1 & B0_2); // (B0_1 ^ B0_2); 

	SOL1_BYTE nF1 = IC_74LS04((nM & B0_1) | (nM & B0_2 & Cn)) ^ (nB1_1 & B1_2); //(B1_1 ^ B1_2);

	SOL1_BYTE nF2 = IC_74LS04((nM & B1_1) | (nM & B0_1 & B1_2) | (nM & Cn & B0_2 & B1_2)) ^ (nB2_1 & B2_2);  //(B2_1 ^ B2_2); 

	SOL1_BYTE nF3 = IC_74LS04((nM & B2_1) | (nM & B1_1 & B2_2) | (nM & B0_1 & B1_2 & B2_2) | (nM & Cn & B0_2 & B1_2 & B2_2)) ^ (nB3_1 & B3_2); // (B3_1 ^ B3_2)

	*AeqB = get_byte_bit(nF0 & nF1 & nF2 & nF3, 0);
	*nP = get_byte_bit(IC_74LS04(B0_2 & B1_2 & B2_2 & B3_2), 0);
	*nG = get_byte_bit(IC_74LS04((B0_1 & B1_2 & B2_2 & B3_2) | (B1_1 & B2_2 & B3_2) | (B2_1 & B3_2) | B3_1), 0);

	*Cn4 = get_byte_bit((Cn & B0_2 & B1_2 & B2_2 & B3_2) | IC_74LS04(*nG), 0);
	SOL1_BYTE nF = (nF3 << 3) | (nF2 << 2) | (nF1 << 1) | (nF0);
	return nF;
}

static void sol1_alu_4bit_reset(struct sol1_alu_4bit *alu) {

	alu->_A = 0x00;
	alu->_B = 0x00;
	alu->_C = 0x00;

	alu->A = 0x00;
	alu->B = 0x00;

	alu->C = 0x00;
	alu->alu_output = 0x00;

	alu->CIN = 0x00; //carry in

	alu->COUT = 0x00; //carry in

	alu->EQ = 0x00;
	alu->P = 0x00;
	alu->G = 0x00;

}
void sol1_alu_4bit_init(struct sol1_alu_4bit *alu) {

	sol1_alu_4bit_reset(alu);

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




//74LS181
SOL1_BYTE sol1_alu_4bit_op(struct sol1_alu_4bit* alu, SOL1_BYTE A, SOL1_BYTE B, SOL1_BYTE CIN, SOL1_BYTE S, SOL1_BYTE M) {

	sol1_alu_4bit_reset(alu);

	alu->_A = A;
	alu->_B = B;
	alu->CIN = CIN;


	alu->alu_output = IC74LSL181(A, B, S, M, CIN,
		&alu->EQ, &alu->COUT, &alu->P, &alu->G);

	alu->C = alu->alu_output;
	alu->_C = alu->alu_output;
	

	return alu->alu_output;
}
