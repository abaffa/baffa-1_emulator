#include "sol1_alu.h"
#include "utils.h"

SOL1_BYTE ALU_EXEC(sol1_alu_8bit *alu, SOL1_BYTE x_bus, SOL1_BYTE y_bus,
	SOL1_BYTE u_cf, SOL1_BYTE msw_cf, SOL1_BYTE shift_src, SOL1_BYTE zbus_out_src, SOL1_BYTE DEBUG_ALU) {

	/*
		SOL1_BYTE alu_op,
			SOL1_BYTE alu_mode,
			SOL1_BYTE alu_cf_in_src,
			SOL1_BYTE alu_cf_in_inv,
			SOL1_BYTE alu_cf_out_inv,

		alu->alu_op = alu_op;
		alu->alu_mode = alu_mode;
		alu->alu_cf_in_src = alu_cf_in_src;
		alu->alu_cf_in_inv = alu_cf_in_inv;
		alu->alu_cf_out_inv = alu_cf_out_inv;
		*/

		//SOL1_BYTE z_bus = 0;

	SOL1_BYTE alu_cin = 0x00;

	if ((alu->alu_cf_in_src & 0b00000011) == 0x00)
		alu_cin = 1; // AQUI ESTRANho
		//alu_cin = 0;
		//alu_cin = alu_mode;
	else
		if ((alu->alu_cf_in_src & 0b00000011) == 0x01)
			alu_cin = msw_cf;

		else if ((alu->alu_cf_in_src & 0b00000011) == 0x02)
			alu_cin = u_cf;


		else if ((alu->alu_cf_in_src & 0b00000011) == 0x03)
			alu_cin = 0;


	alu_cin = (alu_cin ^ alu->alu_cf_in_inv) & 0b00000001;
	//sol1_alu_8bit_op(alu, x_bus, y_bus, (~alu_cin) & 0b00000001, alu_op, alu_mode); AQUI ESTRANho
	//sol1_alu_8bit_op(alu, x_bus, y_bus, (alu_cin) & 0b00000001, alu_op, alu_mode);

	sol1_alu_4bit alu41;
	sol1_alu_4bit alu42;


	sol1_alu_4bit_op(&alu41, x_bus & 0b00001111, y_bus & 0b00001111, (alu_cin) & 0b00000001, alu->alu_op, alu->alu_mode);

	sol1_alu_4bit_op(&alu42, (x_bus & 0b11110000) >> 4, (y_bus & 0b11110000) >> 4, (alu41.COUT) & 0b00000001, alu->alu_op, alu->alu_mode);

	alu->A = x_bus; alu->_A = alu->A;
	alu->B = y_bus; alu->_B = alu->B;
	alu->C = (alu41.alu_output & 0b00001111) | ((alu42.alu_output & 0b00001111) << 4); alu->_C = alu->C;
	alu->CIN = (alu_cin) & 0b00000001;
	alu->alu_output = alu->C;
	alu->COUT = alu42.COUT;

	/*
	printf("------------\n");
	printf("alu_op: ");  print_byte_bin(alu_op); printf("\n");
	printf("alu_mode: "); print_byte_bin(alu_mode); printf("\n");
	printf("alu_cin: "); print_byte_bin(alu_cin); printf("\n\n");
	printf("X: "); print_byte_bin(x_bus); printf("\n");
	printf("Y: "); print_byte_bin(y_bus); printf("\n\n");

	printf("alu41-A: "); print_nibble_bin(alu41._A); printf("\n");
	printf("alu41-B: "); print_nibble_bin(alu41._B); printf("r\n");
	printf("alu41-C: "); print_nibble_bin(alu41.C); printf("\n");
	printf("alu41-COUT: "); print_nibble_bin(alu41.COUT); printf("\n\n");

	printf("alu42-A: "); print_nibble_bin(alu42._A); printf("\n");
	printf("alu42-B: "); print_nibble_bin(alu42._B); printf("\n");
	printf("alu42-C: "); print_nibble_bin(alu42.C); printf("\n");
	printf("alu42-COUT: "); print_nibble_bin(alu42.COUT); printf("\n\n");

	printf("C: "); print_byte_bin(alu->C); printf("\n");
	printf("COUT: "); print_byte_bin(alu->COUT); printf("\n\n");
	*/
	alu->alu_cf = alu->COUT;
	//alu->alu_final_cf = alu->COUT; 
	alu->alu_final_cf = (alu->alu_cf ^ (alu->alu_cf_out_inv)) & 0b00000001;



	/////////////////////////////////////////////////////////////////
	// SHIFT
	SOL1_BYTE z_bus = 0x00;

	SOL1_BYTE inIC16 = 0x00;

	if (shift_src == 0x00)
		inIC16 = 0x00;
		
	else if (shift_src == 0x01)
		inIC16 = u_cf;

	else if (shift_src == 0x02)
		inIC16 = msw_cf;

	else if (shift_src == 0x03)
		inIC16 = get_byte_bit(alu->alu_output, 0);

	else if (shift_src == 0x04)
		inIC16 = get_byte_bit(alu->alu_output, 7);

	else if (shift_src == 0x05 ||
		shift_src == 0x06 ||
		shift_src == 0x07)
		inIC16 = 0x01;


	if ((zbus_out_src & 0b00000011) == 0x00)
		z_bus = alu->alu_output;

	else if ((zbus_out_src & 0b00000011) == 0x01)
		z_bus = (alu->alu_output >> 1) | (inIC16 << 7);

	else if ((zbus_out_src & 0b00000011) == 0x02)
		z_bus = (alu->alu_output << 1) | inIC16;

	else if ((zbus_out_src & 0b00000011) == 0x03)
		z_bus = get_byte_bit(alu->alu_output, 7) != 0x00 ? 0b11111111 : 0b00000000;

	/*
	SHL: MSB goes into CF.LSB replaced by 0
	SLA : same as SHL
	ROL : LSB becomes MSB.
	RLC : MSB goes into CF.LSB becomes CF

	SHR : LSB goes into CF.MSB replaced by 0
	SRA : LSB goes into CF.MSB is whatever the previous one was
	ROR : MSB becomes LSB
	RRC : MSB becomes CF.LSB goes into CF*/
	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	// test OF (OVERFLOW)
	///SOL1_BYTE inIC86_1A = IC_74LS86(get_byte_bit(z_bus, 7), get_byte_bit(x_bus, 7));

	//SOL1_BYTE inIC02_0A = IC_74LS02(get_byte_bit(alu_op, 2), get_byte_bit(alu_op, 1));
	//SOL1_BYTE inIC10_0A = IC_74LS10(get_byte_bit(alu_op, 0), get_byte_bit(alu_op, 3), inIC02_0A); //alu_op == add

	//SOL1_BYTE inIC86_1B = IC_74LS86(get_byte_bit(x_bus, 7), get_byte_bit(y_bus, 7)); //ALU_OP == SUB
	//SOL1_BYTE inIC86_1C = IC_74LS86(inIC10_0A, inIC86_1B);

	//SOL1_BYTE inIC10_0B = IC_74LS10(inIC86_1A, 0x01, IC_74LS04(inIC86_1C));

	//alu->alu_of = IC_74LS04(inIC10_0B);

	SOL1_BYTE zNEQx = (get_byte_bit(z_bus, 7) != get_byte_bit(x_bus, 7));
	SOL1_BYTE xNEQy = (get_byte_bit(x_bus, 7) != get_byte_bit(y_bus, 7));

		alu->alu_of = zNEQx && ((alu->alu_op != 0b1001) == xNEQy);
	//
	/////////////////////////////////////////////////////////////////
	//
	alu->alu_zf = (z_bus == 0x00);

	if (DEBUG_ALU) {
		printf("***** ALU\n");
		sol1_alu_display_registers(alu);

		if ((alu->alu_cf_in_src & 0b00000011) == 0x00)
			//alu_cin = 1; // AQUI ESTRANho
			printf("* alu_cin = 0\n");

		else if ((alu->alu_cf_in_src & 0b00000011) == 0x01) {
			printf("* alu_cin = msw_cf:"); print_byte_bin(msw_cf); printf("\n");
		}
		else if ((alu->alu_cf_in_src & 0b00000011) == 0x02) {
			printf("* alu_cin = u_cf:"); print_byte_bin(u_cf); printf("\n");
		}
		else if ((alu->alu_cf_in_src & 0b00000011) == 0x03)
			printf("* alu_cin = 1\n");

		printf("* z_bus="); print_byte_bin(z_bus);
		printf("\n");
		printf("\n");
	}

	return z_bus;
}
