#include "sol1_bus.h"
#include "utils.h"


SOL1_BYTE SOL1_BUS::bus_tristate(SOL1_REGISTERS& sol1_registers) {
	return get_byte_bit(sol1_registers.MSWl.value(), MSWl_DMA_ACK) | get_byte_bit(sol1_registers.MSWl.value(), MSWl_HALT); //IC151
}

SOL1_BYTE SOL1_BUS::bus_rd(SOL1_REGISTERS& sol1_registers, SOL1_BYTE rd) {

	if (bus_tristate(sol1_registers) != 0x00) return 0x01;

	return (~rd) & 0b00000001;
}

SOL1_BYTE SOL1_BUS::bus_wr(SOL1_REGISTERS& sol1_registers, SOL1_BYTE wr) {
	
	if (bus_tristate(sol1_registers) != 0x00) return 0x01;

	return (~wr) & 0b00000001;
}


void SOL1_BUS::reset() {
	this->data_bus = 0b00000000;
	this->k_bus = 0b00000000; // input pra alu x e y
	this->w_bus = 0b00000000; // input pra alu x e y
	this->x_bus = 0b00000000; //alu entrada
	this->y_bus = 0b00000000; //alu entrada
	this->z_bus = 0b00000000; //alu saida
}


SOL1_BYTE SOL1_BUS::k_bus_refresh(SOL1_REGISTERS& sol1_registers, SOL1_BYTE alu_b_src) {

	//IC92 //I103 //IC116

	SOL1_BYTE k_bus = 0x00;
	switch (alu_b_src & 0b00000011) {
	case 0x00: k_bus = sol1_registers.MDRl.value(); break;
	case 0x01: k_bus = sol1_registers.MDRh.value(); break;
	case 0x02: k_bus = sol1_registers.TDRl.value(); break;
	case 0x03: k_bus = sol1_registers.TDRh.value(); break;
	}

	return k_bus;
}

SOL1_BYTE SOL1_BUS::w_bus_refresh(
	SOL1_REGISTERS& sol1_registers,
	SOL1_BYTE panel_regsel,
	SOL1_BYTE alu_a_src,
	SOL1_BYTE display_reg_load,
	SOL1_BYTE int_vector,
	SOL1_BYTE int_masks,
	SOL1_BYTE int_status,
	FILE *fa, SOL1_BYTE DEBUG_RDREG, HW_TTY& hw_tty) {

	//IC125 //IC118 //IC3   //IC2  //IC9  //IC42  //IC6   //IC20 //IC5  //IC80  //IC41  //IC44
	//IC30  //IC130 //IC56  //IC62 //IC53 //IC133 //IC68  //IC69 //IC67 //IC141 //IC81
	//IC82  //IC71  //IC144 //IC85 //IC86 //IC84  //IC152 //IC88 //IC89 //IC86  //IC160

	char str_out[255];

	SOL1_BYTE w_bus = 0x00;

	SOL1_BYTE inABC = 0x00;
	SOL1_BYTE inAB = 0x00;

	if (bus_tristate(sol1_registers) == 0x00 & display_reg_load == 0x00) {
		inABC = alu_a_src & 0b00000111;
		inAB = get_byte_bit(alu_a_src, 3) | set_byte_bit(get_byte_bit(alu_a_src, 4), 1);
	}
	else {
		inABC = panel_regsel & 0b00000111;
		inAB = get_byte_bit(panel_regsel, 3) | set_byte_bit(get_byte_bit(panel_regsel, 4), 1);
	}

	if (inAB == 0x00) {
		switch (inABC) {
		case 0x00:
			w_bus = sol1_registers.Al.value();
			if (DEBUG_RDREG) { reg8bit_print(fa, (char *)"READ ", (char *)"Al", w_bus); }
			break;
		case 0x01:
			w_bus = sol1_registers.Ah.value();
			if (DEBUG_RDREG) { reg8bit_print(fa, (char *)"READ ", (char *)"Ah", w_bus); }
			break;
		case 0x02:
			w_bus = sol1_registers.Bl.value();
			if (DEBUG_RDREG) { reg8bit_print(fa, (char *)"READ ", (char *)"Bl", w_bus); }
			break;
		case 0x03:
			w_bus = sol1_registers.Bh.value();
			if (DEBUG_RDREG) { reg8bit_print(fa, (char *)"READ ", (char *)"Bh", w_bus); }
			break;
		case 0x04:
			w_bus = sol1_registers.Cl.value();
			if (DEBUG_RDREG) { reg8bit_print(fa, (char *)"READ ", (char *)"Cl", w_bus); }
			break;
		case 0x05:
			w_bus = sol1_registers.Ch.value();
			if (DEBUG_RDREG) { reg8bit_print(fa, (char *)"READ ", (char *)"Ch", w_bus); }
			break;
		case 0x06:
			w_bus = sol1_registers.Dl.value();
			if (DEBUG_RDREG) { reg8bit_print(fa, (char *)"READ ", (char *)"Dl", w_bus); }
			break;
		case 0x07:
			w_bus = sol1_registers.Dh.value();
			if (DEBUG_RDREG) { reg8bit_print(fa, (char *)"READ ", (char *)"Dh", w_bus); }
			break;
		}

	}
	else if (inAB == 0x01) {
		switch (inABC) {
		case 0x00:
			w_bus = sol1_registers.SPl.value();
			if (DEBUG_RDREG) { reg8bit_print(fa, (char *)"READ ", (char *)"SPl", w_bus); }
			break;
		case 0x01:
			w_bus = sol1_registers.SPh.value();
			if (DEBUG_RDREG) { reg8bit_print(fa, (char *)"READ ", (char *)"SPh", w_bus); }
			break;
		case 0x02:
			w_bus = sol1_registers.BPl.value();
			if (DEBUG_RDREG) { reg8bit_print(fa, (char *)"READ ", (char *)"BPl", w_bus); }
			break;
		case 0x03:
			w_bus = sol1_registers.BPh.value();
			if (DEBUG_RDREG) { reg8bit_print(fa, (char *)"READ ", (char *)"BPh", w_bus); }
			break;
		case 0x04:
			w_bus = sol1_registers.SIl.value();
			if (DEBUG_RDREG) { reg8bit_print(fa, (char *)"READ ", (char *)"SIl", w_bus); }
			break;
		case 0x05:
			w_bus = sol1_registers.SIh.value();
			if (DEBUG_RDREG) { reg8bit_print(fa, (char *)"READ ", (char *)"SIh", w_bus); }
			break;
		case 0x06:
			w_bus = sol1_registers.DIl.value();
			if (DEBUG_RDREG) { reg8bit_print(fa, (char *)"READ ", (char *)"DIl", w_bus); }
			break;
		case 0x07:
			w_bus = sol1_registers.DIh.value();
			if (DEBUG_RDREG) { reg8bit_print(fa, (char *)"READ ", (char *)"DIh", w_bus); }
			break;
		}
	}
	else if (inAB == 0x02) {
		switch (inABC) {
		case 0x00:
			w_bus = sol1_registers.PCl.value();
			if (DEBUG_RDREG) { reg8bit_print(fa, (char *)"READ ", (char *)"PCl", w_bus); }
			break;
		case 0x01:
			w_bus = sol1_registers.PCh.value();
			if (DEBUG_RDREG) { reg8bit_print(fa, (char *)"READ ", (char *)"PCh", w_bus); }
			break;
		case 0x02:
			w_bus = sol1_registers.MARl.value();
			if (DEBUG_RDREG) { reg8bit_print(fa, (char *)"READ ", (char *)"MARl", w_bus); }
			break;
		case 0x03:
			w_bus = sol1_registers.MARh.value();
			if (DEBUG_RDREG) { reg8bit_print(fa, (char *)"READ ", (char *)"MARh", w_bus); }
			break;
		case 0x04:
			w_bus = sol1_registers.MDRl.value();
			if (DEBUG_RDREG) { reg8bit_print(fa, (char *)"READ ", (char *)"MDRl", w_bus); }
			break;
		case 0x05:
			w_bus = sol1_registers.MDRh.value();
			if (DEBUG_RDREG) { reg8bit_print(fa, (char *)"READ ", (char *)"MDRh", w_bus); }
			break;
		case 0x06:
			w_bus = sol1_registers.TDRl.value();
			if (DEBUG_RDREG) { reg8bit_print(fa, (char *)"READ ", (char *)"TDRl", w_bus); }
			break;
		case 0x07:
			w_bus = sol1_registers.TDRh.value();
			if (DEBUG_RDREG) { reg8bit_print(fa, (char *)"READ ", (char *)"TDRh", w_bus); }
			break;
		}
	}
	else if (inAB == 0x03) {
		switch (inABC) {
		case 0x00:
			w_bus = sol1_registers.SSPl.value();
			if (DEBUG_RDREG) { reg8bit_print(fa, (char *)"READ ", (char *)"SSPl", w_bus); }
			break;
		case 0x01:
			w_bus = sol1_registers.SSPh.value();
			if (DEBUG_RDREG) { reg8bit_print(fa, (char *)"READ ", (char *)"SSPh", w_bus); }
			break;
		case 0x02:
			w_bus = int_vector;
			if (DEBUG_RDREG) { reg8bit_print(fa, (char *)"READ ", (char *)"INT_VECTOR", w_bus); }
			break;
		case 0x03:
			w_bus = int_masks;
			if (DEBUG_RDREG) { reg8bit_print(fa, (char *)"READ ", (char *)"INT_MASKS", w_bus); }
			break;
		case 0x04:
			w_bus = int_status;
			if (DEBUG_RDREG) { reg8bit_print(fa, (char *)"READ ", (char *)"INT_STATUS", w_bus); }
			break;
		}
	}
	return w_bus;
}


SOL1_BYTE SOL1_BUS::x_bus_refresh(SOL1_REGISTERS& sol1_registers,
	SOL1_BYTE alu_a_src, SOL1_BYTE w_bus) {

	SOL1_BYTE x_bus = 0x00;

	if (!check_byte_bit(alu_a_src, 5))
		x_bus = w_bus;

	else {

		switch ((alu_a_src & 0b00000011)) {
		case 0x00:

			x_bus = set_byte_bit(get_byte_bit(sol1_registers.MSWh.value(), MSWh_ZF), 0) |
				set_byte_bit(get_byte_bit(sol1_registers.MSWh.value(), MSWh_CF), 1) |
				set_byte_bit(get_byte_bit(sol1_registers.MSWh.value(), MSWh_SF), 2) |
				set_byte_bit(get_byte_bit(sol1_registers.MSWh.value(), MSWh_OF), 3) |
				set_byte_bit(get_byte_bit(sol1_registers.MSWh.value(), MSWh_12), 4) |
				set_byte_bit(get_byte_bit(sol1_registers.MSWh.value(), MSWh_13), 5) |
				set_byte_bit(get_byte_bit(sol1_registers.MSWh.value(), MSWh_14), 6) |
				set_byte_bit(get_byte_bit(sol1_registers.MSWh.value(), MSWh_15), 7);
			break;

		case 0x01:

			x_bus = get_byte_bit(sol1_registers.MSWl.value(), MSWl_DMA_ACK) |
				set_byte_bit(get_byte_bit(sol1_registers.MSWl.value(), MSWl_INTERRUPT_ENABLE), 1) |
				set_byte_bit(get_byte_bit(sol1_registers.MSWl.value(), MSWl_CPU_MODE), 2) |
				set_byte_bit(get_byte_bit(sol1_registers.MSWl.value(), MSWl_PAGING_EN), 3) |
				set_byte_bit(get_byte_bit(sol1_registers.MSWl.value(), MSWl_HALT), 4) |
				set_byte_bit(get_byte_bit(sol1_registers.MSWl.value(), MSWl_DISPLAY_REG_LOAD), 5) |
				set_byte_bit(get_byte_bit(sol1_registers.MSWl.value(), MSWl_14), 6) |
				set_byte_bit(get_byte_bit(sol1_registers.MSWl.value(), MSWl_DIR), 7);
			break;

		case 0x02:
			x_bus = sol1_registers.Gl.value();
			break;

		case 0x03:
			x_bus = sol1_registers.Gh.value();
			break;
		}
	}

	return x_bus;
}