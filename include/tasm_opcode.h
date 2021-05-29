#ifndef TASM_OPCODESOL1COMPUTER_H
#define SOL1COMPUTER_H

#include <string>
using namespace std;

class Tasm_Opcode
{
public:
	string opcode;
	string desc;
	int size;

	Tasm_Opcode() {};
	Tasm_Opcode(string opcode, string desc, int size);
};


#endif