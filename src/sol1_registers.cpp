
#include "sol1_registers.h"




SOL1_REGISTERS::SOL1_REGISTERS() {}


SOL1_DWORD SOL1_REGISTERS::value(SOL1_REGISTER_8BIT& l, SOL1_REGISTER_8BIT& h) {

	return l.value() | (h.value() << 8);
}

void SOL1_REGISTERS::set(SOL1_REGISTER_8BIT& l, SOL1_REGISTER_8BIT& h, SOL1_DWORD v) {
	l.set((SOL1_BYTE)(v & 0b11111111));
	h.set((SOL1_BYTE)((v >> 8) & 0b11111111));
}

void SOL1_REGISTERS::reset(SOL1_REGISTER_8BIT& l, SOL1_REGISTER_8BIT& h) {
	l.reset();
	h.reset();
}
