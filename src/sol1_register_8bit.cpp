#include<stdlib.h>
#include "sol1_register_8bit.h"



SOL1_BYTE SOL1_REGISTER_8BIT::value() {
	return this->reg;
}

void SOL1_REGISTER_8BIT::reset() {
	this->reg = 0x0;
}

void SOL1_REGISTER_8BIT::set(SOL1_BYTE v) {
	this->reg = v;
}