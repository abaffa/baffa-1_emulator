#include<stdlib.h>
#include "sol1_register_8bit.h"

struct sol1_register_8bit
{
	SOL1_BYTE reg;
};


SOL1_REGISTER_8BIT *sol1_register_8bit_create() {
	SOL1_REGISTER_8BIT *reg = (SOL1_REGISTER_8BIT*)malloc(sizeof(SOL1_REGISTER_8BIT));
	reg->reg = 0x0;
	return reg;
}

void sol1_register_8bit_destroy(SOL1_REGISTER_8BIT *reg) {
	free(reg);
}


SOL1_BYTE sol1_register_8bit_value(SOL1_REGISTER_8BIT *reg) {
	return reg->reg;
}

void sol1_register_8bit_reset(SOL1_REGISTER_8BIT *reg) {
	reg->reg = 0x0;
}

void sol1_register_8bit_set(SOL1_REGISTER_8BIT *reg, SOL1_BYTE v) {
	reg->reg = v;
}