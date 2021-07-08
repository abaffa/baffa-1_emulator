//
// sol1_alu.cpp
//
////// BEGIN LICENSE NOTICE//////
//
//Sol-1 HomebrewCPU Minicomputer System Emulator
//
//Copyright(C) 2021 Augusto Baffa, (sol-1.baffasoft.com.br)
//
//This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110 - 1301, USA.
//
////// END LICENSE NOTICE//////
//
#include "sol1_alu.h"
#include "utils.h"

static void sol1_alu_reset(struct sol1_alu *alu) {

	alu->_A = 0x00;
	alu->_B = 0x00;
	alu->_C = 0x00;

	alu->A = 0x00;
	alu->B = 0x00;

	alu->C = 0x00;

	alu->CIN = 0x00; //carry in

	alu->COUT = 0x00; //carry in

	alu->EQ = 0x00; //carry in
	alu->F = 0x00; //Larger, equal, zero, carry out

}

void sol1_alu_init(struct sol1_alu *alu) {

	memset(alu, 0, sizeof(struct sol1_alu));

	sol1_alu_reset(alu);

}

SOL1_BYTE ALU_EXEC(sol1_alu *alu, struct sol1_controller_bus *controller_bus, struct sol1_alu_bus *alu_bus,
	SOL1_BYTE u_cf, SOL1_BYTE msw_cf, SOL1_CONFIG& config, HW_TTY& hw_tty) {

	SOL1_BYTE alu_cin = 0x00;

	if ((controller_bus->alu_cf_in_src & 0b00000011) == 0x00)
		alu_cin = 1;
	else
		if ((controller_bus->alu_cf_in_src & 0b00000011) == 0x01)
			alu_cin = msw_cf;

		else if ((controller_bus->alu_cf_in_src & 0b00000011) == 0x02)
			alu_cin = u_cf;


		else if ((controller_bus->alu_cf_in_src & 0b00000011) == 0x03)
			alu_cin = 0;


	alu_cin = (alu_cin ^ controller_bus->alu_cf_in_inv) & 0b00000001;


	sol1_alu_4bit alu41;
	sol1_alu_4bit alu42;


	sol1_alu_4bit_op(&alu41, alu_bus->x_bus & 0b00001111, alu_bus->y_bus & 0b00001111, (alu_cin) & 0b00000001, controller_bus->alu_op, controller_bus->alu_mode);

	sol1_alu_4bit_op(&alu42, (alu_bus->x_bus & 0b11110000) >> 4, (alu_bus->y_bus & 0b11110000) >> 4, (alu41.COUT) & 0b00000001, controller_bus->alu_op, controller_bus->alu_mode);

	alu->A = alu_bus->x_bus; alu->_A = alu->A;
	alu->B = alu_bus->y_bus; alu->_B = alu->B;
	alu->C = (alu41.alu_output & 0b00001111) | ((alu42.alu_output & 0b00001111) << 4); alu->_C = alu->C;
	alu->CIN = (alu_cin) & 0b00000001;
	alu_bus->alu_output = alu->C;
	alu->COUT = alu42.COUT;

	alu_bus->alu_cf = alu->COUT;
	alu_bus->alu_final_cf = (alu_bus->alu_cf ^ (controller_bus->alu_cf_out_inv)) & 0b00000001;


	/////////////////////////////////////////////////////////////////
	// SHIFT
	alu_bus->z_bus = 0x00;

	SOL1_BYTE inIC16 = 0x00;

	if (controller_bus->shift_src == 0x00)
		inIC16 = 0x00;

	else if (controller_bus->shift_src == 0x01)
		inIC16 = u_cf;

	else if (controller_bus->shift_src == 0x02)
		inIC16 = msw_cf;

	else if (controller_bus->shift_src == 0x03)
		inIC16 = get_byte_bit(alu_bus->alu_output, 0);

	else if (controller_bus->shift_src == 0x04)
		inIC16 = get_byte_bit(alu_bus->alu_output, 7);

	else if (controller_bus->shift_src == 0x05 ||
		controller_bus->shift_src == 0x06 ||
		controller_bus->shift_src == 0x07)
		inIC16 = 0x01;

	
	if ((controller_bus->zbus_out_src & 0b00000011) == 0x00)
		alu_bus->z_bus = alu_bus->alu_output;

	else if ((controller_bus->zbus_out_src & 0b00000011) == 0x01)
		alu_bus->z_bus = (alu_bus->alu_output >> 1) | (inIC16 << 7);

	else if ((controller_bus->zbus_out_src & 0b00000011) == 0x02)
		alu_bus->z_bus = (alu_bus->alu_output << 1) | inIC16;

	else if ((controller_bus->zbus_out_src & 0b00000011) == 0x03)
		alu_bus->z_bus = get_byte_bit(alu_bus->alu_output, 7) != 0x00 ? 0b11111111 : 0b00000000;

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

	SOL1_BYTE zNEQx = (get_byte_bit(alu_bus->z_bus, 7) != get_byte_bit(alu_bus->x_bus, 7));
	SOL1_BYTE xNEQy = (get_byte_bit(alu_bus->x_bus, 7) != get_byte_bit(alu_bus->y_bus, 7));

	alu_bus->alu_of = zNEQx && ((controller_bus->alu_op != 0b1001) == xNEQy);
	//
	/////////////////////////////////////////////////////////////////
	//
	alu_bus->alu_zf = (alu_bus->z_bus == 0x00);

	if (config.DEBUG_ALU) {
		char str_out[255];
		hw_tty.print("***** ALU\n");
		sol1_alu_display_registers(alu, controller_bus, alu_bus, hw_tty);

		if ((controller_bus->alu_cf_in_src & 0b00000011) == 0x00)
			hw_tty.print("* alu_cin = 0\n");

		else if ((controller_bus->alu_cf_in_src & 0b00000011) == 0x01) {
			hw_tty.print("* alu_cin = msw_cf:"); print_byte_bin(str_out, msw_cf); hw_tty.print(str_out); hw_tty.print("\n");
		}
		else if ((controller_bus->alu_cf_in_src & 0b00000011) == 0x02) {
			hw_tty.print("* alu_cin = u_cf:"); print_byte_bin(str_out, u_cf); hw_tty.print(str_out); hw_tty.print("\n");
		}
		else if ((controller_bus->alu_cf_in_src & 0b00000011) == 0x03)
			hw_tty.print("* alu_cin = 1\n");

		hw_tty.print("* z_bus="); print_byte_bin(str_out, alu_bus->z_bus); hw_tty.print(str_out);
		hw_tty.print("\n");
		hw_tty.print("\n");
	}

	return alu_bus->z_bus;
}


void sol1_alu_display_registers(struct sol1_alu *alu, struct sol1_controller_bus *controller_bus, struct sol1_alu_bus *alu_bus, HW_TTY& hw_tty) {

	char str_out[255];
	hw_tty.print("* A:"); print_byte_bin(str_out, alu->_A); hw_tty.print(str_out); hw_tty.print("  | ");
	hw_tty.print("B:"); print_byte_bin(str_out, alu->_B); hw_tty.print(str_out); hw_tty.print("    | ");
	hw_tty.print("C:"); print_byte_bin(str_out, alu->_C); hw_tty.print(str_out); hw_tty.print("  | ");
	hw_tty.print("Cin:"); print_nibble_bin(str_out, alu->CIN); hw_tty.print(str_out); hw_tty.print(" | ");
	hw_tty.print("Cout:"); print_nibble_bin(str_out, alu->COUT); hw_tty.print(str_out);
	hw_tty.print("\n");

	hw_tty.print("* alu_op: "); print_nibble_bin(str_out, controller_bus->alu_op); hw_tty.print(str_out); hw_tty.print(" | alu_mode: "); print_nibble_bin(str_out, controller_bus->alu_mode); hw_tty.print(str_out);
	hw_tty.print(" | alu_output="); print_byte_bin(str_out, alu_bus->alu_output); hw_tty.print(str_out); hw_tty.print("\n");

	//hw_tty.print("* EQ="); print_nibble_bin(str_out, alu->EQ); hw_tty.print(str_out);  hw_tty.print(" | ");	
	//hw_tty.print("F="); print_byte_bin(str_out, alu->F); hw_tty.print(str_out); hw_tty.print("\n");

	hw_tty.print("* Flags: [");

	if (alu_bus->alu_zf != 0x00) hw_tty.print("Z"); else hw_tty.print(" ");
	if (alu_bus->alu_cf != 0x00) hw_tty.print("C"); else hw_tty.print(" ");
	if (alu_bus->alu_of != 0x00) hw_tty.print("O"); else hw_tty.print(" ");

	hw_tty.print("] ");
	sprintf(str_out, " | alu_a_src=%02x", controller_bus->alu_a_src); hw_tty.print(str_out);
	sprintf(str_out, " | alu_b_src=%02x", controller_bus->alu_b_src); hw_tty.print(str_out);
	sprintf(str_out, " | alu_cf_in_src=%02x", controller_bus->alu_cf_in_src); hw_tty.print(str_out);
	sprintf(str_out, " | alu_cf_in_inv=%02x", controller_bus->alu_cf_in_inv != 0x00); hw_tty.print(str_out);
	sprintf(str_out, " | alu_cf_out_inv=%02x", controller_bus->alu_cf_out_inv); hw_tty.print(str_out);
	sprintf(str_out, " | alu_final_cf=%02x", alu_bus->alu_final_cf); hw_tty.print(str_out);
	hw_tty.print("\n");
}
