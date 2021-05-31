#ifndef SOL1BUS_H
#define SOL1BUS_H
#include "config.h"
#include "sol1_registers.h"

class SOL1_BUS {


public:
	SOL1_BYTE data_bus;

	SOL1_BYTE k_bus; // input pra alu x e y
	SOL1_BYTE w_bus; // input pra alu x e y
	SOL1_BYTE x_bus; //alu entrada
	SOL1_BYTE y_bus; //alu entrada
	SOL1_BYTE z_bus; //alu saida


	SOL1_BYTE bus_tristate(SOL1_REGISTERS& sol1_registers);

	SOL1_BYTE bus_rd(SOL1_REGISTERS& sol1_registers, SOL1_BYTE rd);

	SOL1_BYTE bus_wr(SOL1_REGISTERS& sol1_registers, SOL1_BYTE wr);


	SOL1_BYTE k_bus_refresh(SOL1_REGISTERS& sol1_registers, SOL1_BYTE alu_b_src);

	SOL1_BYTE w_bus_refresh(SOL1_REGISTERS& sol1_registers, SOL1_BYTE panel_regsel, SOL1_BYTE alu_a_src, SOL1_BYTE display_reg_load,
		SOL1_BYTE int_vector, SOL1_BYTE int_masks, SOL1_BYTE int_status, FILE *fa, SOL1_BYTE DEBUG_RDREG);

	SOL1_BYTE x_bus_refresh(SOL1_REGISTERS& sol1_registers,
		SOL1_BYTE alu_a_src, SOL1_BYTE w_bus);
};
#endif