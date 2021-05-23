#include "ttl74series.h"
#include "utils.h"



// 2-NOR
SOL1_BYTE IC_74LS02(SOL1_BYTE A, SOL1_BYTE B) {
	return (~(A | B)) & 0b00000001;
}

// INVERTER
SOL1_BYTE IC_74LS04(SOL1_BYTE A) {
	return get_byte_bit(~A, 0);
}


// 2-AND
SOL1_BYTE IC_74LS08(SOL1_BYTE A, SOL1_BYTE B) {
	return (A & B) & 0b00000001;
}


// 3-NAND
SOL1_BYTE IC_74LS10(SOL1_BYTE A, SOL1_BYTE B, SOL1_BYTE C) {
	return (~(A & B & C)) & 0b00000001;
}

// 3-AND
SOL1_BYTE IC_74LS11(SOL1_BYTE A, SOL1_BYTE B, SOL1_BYTE C) {
	return (A & B & C) & 0b00000001;
}

// 3-NOR
SOL1_BYTE IC_74LS27(SOL1_BYTE A, SOL1_BYTE B, SOL1_BYTE C) {
	return (~(A | B | C)) & 0b00000001;
}

// 2-OR
SOL1_BYTE IC_74LS32(SOL1_BYTE A, SOL1_BYTE B) {
	return (A | B) & 0b00000001;
}

// 2-XOR
SOL1_BYTE IC_74LS86(SOL1_BYTE A, SOL1_BYTE B) {
	return (A ^ B) & 0b00000001;
}

//74LS138     3 - line to 8 - line decoder / demultiplexer 16 - PDIP 0 to 70
//74LS148     10 - LINE TO 4 - LINE AND 8 - LINE TO 3 - LINE PRIORITY ENCODERS

//74LS151     1 - of - 8 Line Data Selector / Multiplexer257
SOL1_BYTE IC_74LS151(SOL1_BYTE D,
	SOL1_BYTE ABC,
	SOL1_BYTE G) {

	SOL1_BYTE ret = get_byte_bit(D, ABC);

	if (G == 0x0)
		return ret & 0b00000001 | (((~ret) & 0b00000001) << 1);
	else
		return 0b10;
}

//74LS153     Dual 1 - of - 4 Line Data Selectors / Multiplexer
SOL1_BYTE IC_74LS153(SOL1_BYTE X1,
	SOL1_BYTE X2,
	SOL1_BYTE AB,
	SOL1_BYTE G) {

	SOL1_BYTE Y1 = 0x0;
	SOL1_BYTE Y2 = 0x0;

	if ((G & 0x1) == 0x0) // ~1G
		Y1 = get_byte_bit(X1 >> AB,0);

	if ((G & 0x2) == 0x0) // ~1G
		Y2 = get_byte_bit(X2 >> AB,0);

	return (Y2 << 1) | Y1;
}

//74LS157     QUADRUPLE 2 - LINE TO 1 - LINE DATA SELECTORS / MULTIPLEXERS
SOL1_BYTE IC_74LS157(SOL1_BYTE A,
	SOL1_BYTE B,
	SOL1_BYTE OEAB) {
	SOL1_BYTE ret = 0x0;

	if ((OEAB & 0x2) == 0x0) // ~OE
		if ((OEAB & 0x1) == 0x0) // ~A
			ret = A;

		else  // B
			ret = B;

	return ret & 0b00001111;
}

//74LS244     Octal 3 - STATE Buffer / Line Driver / Line Receiver
SOL1_BYTE IC_74LS244(SOL1_BYTE Y,
	SOL1_BYTE OE) {

	SOL1_BYTE ret = 0x0;

	if ((OE & 0x1) == 0x0) // ~OE
		ret = Y;

	return ret;
}

//74LS253     3 - STATE Data Selector / Multiplexer
SOL1_BYTE IC_74LS253(SOL1_BYTE A,
	SOL1_BYTE B,
	SOL1_BYTE AB,
	SOL1_BYTE G) {

	SOL1_BYTE Y1 = 0x0;
	SOL1_BYTE Y2 = 0x0;

	if ((G & 0x1) == 0x0) // ~1G
		Y1 = get_byte_bit(A >> AB, 0);

	if ((G & 0x2) == 0x0) // ~1G
		Y2 = get_byte_bit(B >> AB, 0);

	return (Y2 << 1) | Y1;
}

//74LS257     3-STATE Quad 2-Data Selectors/Multiplexers
SOL1_BYTE IC_74LS257(SOL1_BYTE A,
	SOL1_BYTE B,
	SOL1_BYTE OEAB) {
	SOL1_BYTE ret = 0x0;

	if ((OEAB & 0x2) == 0x0) // ~OE
		if ((OEAB & 0x1) == 0x0) // ~A
			ret = A;

		else  // B
			ret = B;

	return ret;
}


//74LS273     8-Bit Register with Clear
SOL1_BYTE IC_74LS273(SOL1_REGISTER_8BIT *reg, SOL1_BYTE D, SOL1_BYTE CLK, SOL1_BYTE MR) {
	//reset low resets values, high changes values
	if ((MR & 0x01) == 0x0)
		sol1_register_8bit_reset(reg);

	else if ((CLK & 0x01) != 0x0)
		sol1_register_8bit_set(reg, D);


	return sol1_register_8bit_value(reg);
}


//74LS377     Octal D - Type Flip - Flop with Common Enable and Clock
SOL1_BYTE IC_74LS377(SOL1_REGISTER_8BIT *reg, SOL1_BYTE D, SOL1_BYTE CLK, SOL1_BYTE G) {
	//enable low changes values, high only persists (dont change)

	if ((G & 0x01) == 0x0 && (CLK & 0x01) != 0x0)
		sol1_register_8bit_set(reg, D);

	return sol1_register_8bit_value(reg);
}


////////////////////
////////////////////////////////////////
////////////////////
////////////////////

SOL1_BYTE IC74LS181_B1(SOL1_BYTE nA, SOL1_BYTE nB, SOL1_BYTE S, int block) {

	SOL1_BYTE S0 = get_byte_bit(S, 0);
	SOL1_BYTE S1 = get_byte_bit(S, 1);

	SOL1_BYTE andA = get_byte_bit(nA, block);
	SOL1_BYTE andB = get_byte_bit(nB, block) & S0;
	SOL1_BYTE andC = S1 & IC_74LS04(get_byte_bit(nB, block));

	return IC_74LS04(andA | andB | andC);
}

SOL1_BYTE IC74LS181_B2(SOL1_BYTE nA, SOL1_BYTE nB, SOL1_BYTE S, int block) {

	SOL1_BYTE S2 = get_byte_bit(S, 2);
	SOL1_BYTE S3 = get_byte_bit(S, 3);

	SOL1_BYTE andA = IC_74LS04(get_byte_bit(nB, block)) & S2 & get_byte_bit(nA, block);
	SOL1_BYTE andB = get_byte_bit(nA, block) & get_byte_bit(nB, block) & S3;

	return IC_74LS04(andA | andB);

}

SOL1_BYTE IC74LSL181(SOL1_BYTE nA, SOL1_BYTE nB, SOL1_BYTE S, SOL1_BYTE M, SOL1_BYTE Cn,
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