#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "debugmenu_main.h"
#include "utils.h"



void debugmenu_main_menu() {
	printf("\n");
	printf("SOL-1 Debug Monitor | XX May 2021\n");
	printf("\n");
	printf("  D - Display Memory\n");
	printf("  I - Disassemble Memory\n");
	printf("  E - Edit Memory\n");
	printf("  F - Fill Memory\n");
	printf("  L - Load Memory\n");
	printf("\n");
	printf("  R - Display Registers\n");
	printf("  A - Edit Register\n");
	printf("  B - Edit Breakpoint\n");
	printf("  P - Edit Program Counter\n");
	printf("\n");
	printf("  G - Go(Run)\n");
	printf("  S - Step\n");
	printf("  Z - Reset CPU\n");
	printf("\n");
	printf("  ? - Display Menu\n");
	printf("  Q - Quit\n");
	printf("\n");
}


void debugmenu_main_disassemble_mem() {
}


void debugmenu_main_edit_mem(struct sol1_memory* memory) {

	char *input;
	char *value = (char*)malloc(sizeof(char) * 257);

	printf("Edit Memory | Address? ");
	input = gets(4);

	if (strlen(input) == 0) { printf("\n");  return; }

	leftpad(input, value, 4);

	unsigned short address = convert_hexstr_to_value(value);
	printf("\n  %04x=%02x ? ", address, memory->memory[address]);
	input = gets(2);

	while (strlen(input) > 0) {

		leftpad(input, value, 2);

		unsigned char b = convert_hexstr_to_value(value);

		memory->memory[address] = b;
		address++;
		if (address >= SOL1_MEMORY_SIZE) { printf("\n");  return; }

		printf("\n  %04x=%02x ? ", address, memory->memory[address]);
		input = gets(2);
	}

	printf("\n");
}



void debugmenu_main_fill_mem(struct sol1_memory* memory) {

	char *input;
	char *value = (char*)malloc(sizeof(char) * 257);
	unsigned short address, start, end;
	unsigned char data;

	printf("Fill Memory | Start Address? ");
	input = gets(4);

	if (strlen(input) == 0) { printf("\n");  return; }

	leftpad(input, value, 4);
	start = convert_hexstr_to_value(value);

	printf(" | End Address? ");
	input = gets(4);

	if (strlen(input) == 0) { printf("\n");  return; }

	leftpad(input, value, 4);
	end = convert_hexstr_to_value(value);

	printf(" | Data? ");
	input = gets(2);

	if (strlen(input) == 0) { printf("\n");  return; }

	leftpad(input, value, 2);
	data = convert_hexstr_to_value(value);

	for (address = start; address <= end && address < SOL1_MEMORY_SIZE; address++)
		memory->memory[address] = data;

	printf("\n");
}




void debugmenu_main_load_mem(struct sol1_cpu *sol1_cpu) {

	char *input;
	char *value = (char*)malloc(sizeof(char) * 257);
	unsigned short address, start, end;
	unsigned char data;

	printf("Load Memory | Start Address? ");
	input = gets(4);

	if (strlen(input) == 0) { printf("\n");  return; }

	leftpad(input, value, 4);
	start = convert_hexstr_to_value(value);


	printf("\n  Code (max 128 bytes) ] ");
	input = gets(256);

	if (strlen(input) == 0) { printf("\n");  return; }

	size_t numdigits = strlen(input) / 2;
	size_t i;
	for (i = 0; i != numdigits; ++i)
	{
		unsigned char output = 16 * toInt(input[2 * i]) + toInt(input[2 * i + 1]);
		sol1_cpu->memory.memory[i + start] = output;
	}


	sol1_cpu->registers.PC = start;

	printf("\n");
}



void debugmenu_main_display_registers(struct sol1_cpu *sol1_cpu) {

	printf("Display Registers\n");
	printf("\n");
	sol1_cpu_display_registers(sol1_cpu);
}

void debugmenu_main_edit_register(struct sol1_cpu *sol1_cpu) {

	char *input;
	char *value = (char*)malloc(sizeof(char) * 257);
	unsigned short address, start, end;
	unsigned char data;

	printf("Edit Register ? ");
	input = gets(3);

	if (strlen(input) == 0) { printf("\n");  return; }

	if (strcmp(input, "A") == 0) {
		printf(" | A=%04x | Data? ", sol1_cpu->registers.A);
		input = gets(4);

		if (strlen(input) == 0) { printf("\n");  return; }

		leftpad(input, value, 4);
		sol1_cpu->registers.A = convert_hexstr_to_value(value);
	}
	else if (strcmp(input, "B") == 0) {
		printf(" | B=%04x | Data? ", sol1_cpu->registers.B);
		input = gets(4);

		if (strlen(input) == 0) { printf("\n");  return; }

		leftpad(input, value, 4);
		sol1_cpu->registers.B = convert_hexstr_to_value(value);
	}
	else if (strcmp(input, "C") == 0) {
		printf(" | C=%04x | Data? ", sol1_cpu->registers.C);
		input = gets(4);

		if (strlen(input) == 0) { printf("\n");  return; }

		leftpad(input, value, 4);
		sol1_cpu->registers.C = convert_hexstr_to_value(value);
	}
	else if (strcmp(input, "D") == 0) {
		printf(" | D=%04x | Data? ", sol1_cpu->registers.D);
		input = gets(4);

		if (strlen(input) == 0) { printf("\n");  return; }

		leftpad(input, value, 4);
		sol1_cpu->registers.D = convert_hexstr_to_value(value);
	}
	else if (strcmp(input, "G") == 0) {
		printf(" | B=%04x | Data? ", sol1_cpu->registers.G);
		input = gets(4);

		if (strlen(input) == 0) { printf("\n");  return; }

		leftpad(input, value, 4);
		sol1_cpu->registers.G = convert_hexstr_to_value(value);
	}

	else if (strcmp(input, "BP") == 0) {
		printf(" | BP=%04x | Data? ", sol1_cpu->registers.BP);
		input = gets(4);

		if (strlen(input) == 0) { printf("\n");  return; }

		leftpad(input, value, 4);
		sol1_cpu->registers.BP = convert_hexstr_to_value(value);
	}
	else if (strcmp(input, "SP") == 0) {
		printf(" | SP=%04x | Data? ", sol1_cpu->registers.SP);
		input = gets(4);

		if (strlen(input) == 0) { printf("\n");  return; }

		leftpad(input, value, 4);
		sol1_cpu->registers.SP = convert_hexstr_to_value(value);
	}


	else if (strcmp(input, "SI") == 0) {
		printf(" | SI=%04x | Data? ", sol1_cpu->registers.SI);
		input = gets(4);

		if (strlen(input) == 0) { printf("\n");  return; }

		leftpad(input, value, 4);
		sol1_cpu->registers.SI = convert_hexstr_to_value(value);
	}
	else if (strcmp(input, "DI") == 0) {
		printf(" | DI=%04x | Data? ", sol1_cpu->registers.DI);
		input = gets(4);

		if (strlen(input) == 0) { printf("\n");  return; }

		leftpad(input, value, 4);
		sol1_cpu->registers.DI = convert_hexstr_to_value(value);
	}
	else if (strcmp(input, "PC") == 0) {
		printf(" | PC=%04x | Data? ", sol1_cpu->registers.PC);
		input = gets(4);

		if (strlen(input) == 0) { printf("\n");  return; }

		leftpad(input, value, 4);
		sol1_cpu->registers.PC = convert_hexstr_to_value(value);
	}


	else if (strcmp(input, "TDR") == 0) {
		printf(" | TDR=%04x | Data? ", sol1_cpu->registers.TDR);
		input = gets(4);

		if (strlen(input) == 0) { printf("\n");  return; }

		leftpad(input, value, 4);
		sol1_cpu->registers.TDR = convert_hexstr_to_value(value);
	}
	else if (strcmp(input, "SSP") == 0) {
		printf(" | SSP=%04x | Data? ", sol1_cpu->registers.SSP);
		input = gets(4);

		if (strlen(input) == 0) { printf("\n");  return; }

		leftpad(input, value, 4);
		sol1_cpu->registers.SSP = convert_hexstr_to_value(value);
	}
	else if (strcmp(input, "PTB") == 0) {
		printf(" | PTB=%02x | Data? ", sol1_cpu->registers.PC);
		input = gets(2);

		if (strlen(input) == 0) { printf("\n");  return; }

		leftpad(input, value, 2);
		sol1_cpu->registers.PTB = convert_hexstr_to_value(value);
	}

	else if (strcmp(input, "MAR") == 0) {
		printf(" | MAR=%04x | Data? ", sol1_cpu->memory.MAR);
		input = gets(4);

		if (strlen(input) == 0) { printf("\n");  return; }

		leftpad(input, value, 4);
		sol1_cpu->memory.MAR = convert_hexstr_to_value(value);
		sol1_cpu->memory.MDR = sol1_cpu->memory.memory[sol1_cpu->memory.MAR];
	}

	printf("\n");
}

void debugmenu_main_edit_breakpoint(struct sol1_cpu *sol1_cpu) {

	char *input;
	char *value = (char*)malloc(sizeof(char) * 257);

	printf("Edit Breakpoint | BKPT=%04x | Address (FFFF=disable) ? ", sol1_cpu->BKPT);
	input = gets(4);

	if (strlen(input) == 0) { printf("\n");  return; }

	leftpad(input, value, 4);
	sol1_cpu->BKPT = convert_hexstr_to_value(value);

	printf("\n");
}

void debugmenu_main_edit_programcounter(struct sol1_cpu *sol1_cpu) {

	char *input;
	char *value = (char*)malloc(sizeof(char) * 257);

	printf("Edit Program Counter | PC=%04x | Address ? ", sol1_cpu->registers.PC);
	input = gets(4);

	if (strlen(input) == 0) { printf("\n");  return; }

	leftpad(input, value, 4);
	sol1_cpu->registers.PC = convert_hexstr_to_value(value);

	printf("\n");
}

void debugmenu_main_reset_cpu(struct sol1_cpu *sol1_cpu) {


	printf("Reset CPU\n");
	printf("\n");
	sol1_cpu_display_registers(sol1_cpu);
}


