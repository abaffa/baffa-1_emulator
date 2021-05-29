#include "tasm_opcode.h"


Tasm_Opcode::Tasm_Opcode(string opcode, string desc, int size) {
	this->opcode = opcode;
	this->desc = desc;
	this->size = size;
}


