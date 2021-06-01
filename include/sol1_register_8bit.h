#ifndef SOL1REGISTER_8BIT_H
#define SOL1REGISTER_8BIT_H

#include "config.h"

class SOL1_REGISTER_8BIT {


private:
	SOL1_BYTE reg;

public:
	SOL1_REGISTER_8BIT() {
		this->reg = 0x0;
	}

	SOL1_BYTE value();

	void reset();

	void set(SOL1_BYTE v);

};

#endif