#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#include "debugmenu_main.h"
#include "debugmenu_roms.h"
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
	printf("  T - Debug Trace\n");
	printf("  Z - Reset CPU\n");
	printf("\n");
	printf("  X - Debug Roms\n");
	printf("\n");
	printf("  ? - Display Menu\n");
	printf("  Q - Quit\n");
	printf("\n");
}


void debugmenu_main_disassemble_mem() {
}


void debugmenu_main_edit_mem(SOL1_MEMORY& memory) {

	char *input;
	char *value = (char*)malloc(sizeof(char) * 257);

	printf("Edit Memory | Address? ");
	input = gets(4);

	if (strlen(input) == 0) { printf("\n");  return; }

	leftpad(input, value, 4);

	unsigned short address = convert_hexstr_to_value(value);
	printf("\n  %04x=%02x ? ", address, memory.memory[address]);
	input = gets(2);

	while (strlen(input) > 0) {

		leftpad(input, value, 2);

		unsigned char b = convert_hexstr_to_value(value);

		memory.memory[address] = b;
		address++;
		if (address >= SOL1_BIOS_MEMORY_SIZE) { printf("\n");  return; }

		printf("\n  %04x=%02x ? ", address, memory.memory[address]);
		input = gets(2);
	}

	printf("\n");
}



void debugmenu_main_fill_mem(SOL1_MEMORY& memory) {

	char *input;
	char *value = (char*)malloc(sizeof(char) * 257);
	unsigned long address, start, end;
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


	for (address = start; address <= end && address < SOL1_BIOS_MEMORY_SIZE; address++)
		memory.memory[address] = data;

	printf("\n");
}




void debugmenu_main_load_mem(SOL1_CPU& sol1_cpu) {

	char *input;
	char *value = (char*)malloc(sizeof(char) * 257);
	unsigned short start; //address,  end
//	unsigned char data;

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
		sol1_cpu.memory.memory[i + start] = output;
	}

	SOL1_REGISTERS::set(sol1_cpu.registers.PCl, sol1_cpu.registers.PCh, start);

	printf("\n");
}



void debugmenu_main_display_registers(SOL1_CPU& sol1_cpu) {

	printf("Display Registers\n");
	printf("\n");
	sol1_cpu.display_registers();
}

void debugmenu_main_edit_register(SOL1_CPU& sol1_cpu) {

	char *input;
	char *value = (char*)malloc(sizeof(char) * 257);
	//unsigned short address; // , end
//	unsigned char data;

	printf("Edit Register ? ");
	input = gets(3);

	if (strlen(input) == 0) { printf("\n");  return; }

	if (strcmp(input, "A") == 0) {
		printf(" | A=%04x | Data? ", SOL1_REGISTERS::value(sol1_cpu.registers.Al, sol1_cpu.registers.Ah));
		input = gets(4);

		if (strlen(input) == 0) { printf("\n");  return; }

		leftpad(input, value, 4);
		SOL1_REGISTERS::set(sol1_cpu.registers.Al, sol1_cpu.registers.Ah, convert_hexstr_to_value(value));
	}
	else if (strcmp(input, "B") == 0) {
		printf(" | B=%04x | Data? ", SOL1_REGISTERS::value(sol1_cpu.registers.Bl, sol1_cpu.registers.Bh));
		input = gets(4);

		if (strlen(input) == 0) { printf("\n");  return; }

		leftpad(input, value, 4);
		SOL1_REGISTERS::set(sol1_cpu.registers.Bl, sol1_cpu.registers.Bh, convert_hexstr_to_value(value));
	}
	else if (strcmp(input, "C") == 0) {
		printf(" | C=%04x | Data? ", SOL1_REGISTERS::value(sol1_cpu.registers.Cl, sol1_cpu.registers.Ch));
		input = gets(4);

		if (strlen(input) == 0) { printf("\n");  return; }

		leftpad(input, value, 4);
		SOL1_REGISTERS::set(sol1_cpu.registers.Cl, sol1_cpu.registers.Ch, convert_hexstr_to_value(value));
	}
	else if (strcmp(input, "D") == 0) {
		printf(" | D=%04x | Data? ", SOL1_REGISTERS::value(sol1_cpu.registers.Dl, sol1_cpu.registers.Dh));
		input = gets(4);

		if (strlen(input) == 0) { printf("\n");  return; }

		leftpad(input, value, 4);
		SOL1_REGISTERS::set(sol1_cpu.registers.Dl, sol1_cpu.registers.Dh, convert_hexstr_to_value(value));
	}
	else if (strcmp(input, "G") == 0) {
		printf(" | G=%04x | Data? ", SOL1_REGISTERS::value(sol1_cpu.registers.Gl, sol1_cpu.registers.Gh));
		input = gets(4);

		if (strlen(input) == 0) { printf("\n");  return; }

		leftpad(input, value, 4);
		SOL1_REGISTERS::set(sol1_cpu.registers.Gl, sol1_cpu.registers.Gh, convert_hexstr_to_value(value));
	}


	else if (strcmp(input, "BP") == 0) {
		printf(" | BP=%04x | Data? ", SOL1_REGISTERS::value(sol1_cpu.registers.BPl, sol1_cpu.registers.BPh));
		input = gets(4);

		if (strlen(input) == 0) { printf("\n");  return; }

		leftpad(input, value, 4);
		SOL1_REGISTERS::set(sol1_cpu.registers.BPl, sol1_cpu.registers.BPh, convert_hexstr_to_value(value));
	}
	else if (strcmp(input, "SP") == 0) {
		printf(" | SP=%04x | Data? ", SOL1_REGISTERS::value(sol1_cpu.registers.SPl, sol1_cpu.registers.SPh));
		input = gets(4);

		if (strlen(input) == 0) { printf("\n");  return; }

		leftpad(input, value, 4);
		SOL1_REGISTERS::set(sol1_cpu.registers.SPl, sol1_cpu.registers.SPh, convert_hexstr_to_value(value));
	}

	else if (strcmp(input, "SI") == 0) {
		printf(" | SI=%04x | Data? ", SOL1_REGISTERS::value(sol1_cpu.registers.SIl, sol1_cpu.registers.SIh));
		input = gets(4);

		if (strlen(input) == 0) { printf("\n");  return; }

		leftpad(input, value, 4);
		SOL1_REGISTERS::set(sol1_cpu.registers.SIl, sol1_cpu.registers.SIh, convert_hexstr_to_value(value));
	}
	else if (strcmp(input, "DI") == 0) {
		printf(" | DI=%04x | Data? ", SOL1_REGISTERS::value(sol1_cpu.registers.DIl, sol1_cpu.registers.DIh));
		input = gets(4);

		if (strlen(input) == 0) { printf("\n");  return; }

		leftpad(input, value, 4);
		SOL1_REGISTERS::set(sol1_cpu.registers.DIl, sol1_cpu.registers.DIh, convert_hexstr_to_value(value));
	}
	else if (strcmp(input, "PC") == 0) {
		printf(" | PC=%04x | Data? ", SOL1_REGISTERS::value(sol1_cpu.registers.PCl, sol1_cpu.registers.PCh));
		input = gets(4);

		if (strlen(input) == 0) { printf("\n");  return; }

		leftpad(input, value, 4);
		SOL1_REGISTERS::set(sol1_cpu.registers.PCl, sol1_cpu.registers.PCh, convert_hexstr_to_value(value));
	}


	else if (strcmp(input, "TDR") == 0) {
		printf(" | TDR=%04x | Data? ", SOL1_REGISTERS::value(sol1_cpu.registers.TDRl, sol1_cpu.registers.TDRh));
		input = gets(4);

		if (strlen(input) == 0) { printf("\n");  return; }

		leftpad(input, value, 4);
		SOL1_REGISTERS::set(sol1_cpu.registers.TDRl, sol1_cpu.registers.TDRh, convert_hexstr_to_value(value));
	}
	else if (strcmp(input, "SSP") == 0) {
		printf(" | SSP=%04x | Data? ", SOL1_REGISTERS::value(sol1_cpu.registers.SSPl, sol1_cpu.registers.SSPh));
		input = gets(4);

		if (strlen(input) == 0) { printf("\n");  return; }

		leftpad(input, value, 4);
		SOL1_REGISTERS::set(sol1_cpu.registers.SSPl, sol1_cpu.registers.SSPh, convert_hexstr_to_value(value));

	}
	else if (strcmp(input, "PTB") == 0) {
		printf(" | PTB=%02x | Data? ", sol1_cpu.registers.PTB.value());
		input = gets(2);

		if (strlen(input) == 0) { printf("\n");  return; }

		leftpad(input, value, 2);
		sol1_cpu.registers.PTB.set(convert_hexstr_to_value(value));
	}
	else if (strcmp(input, "MSW") == 0) {
		printf(" | MSW=%04x | Data? ", SOL1_REGISTERS::value(sol1_cpu.registers.MSWl, sol1_cpu.registers.MSWh));
		input = gets(4);

		if (strlen(input) == 0) { printf("\n");  return; }

		leftpad(input, value, 4);
		SOL1_REGISTERS::set(sol1_cpu.registers.MSWl, sol1_cpu.registers.MSWh, convert_hexstr_to_value(value));
	}

	else if (strcmp(input, "MAR") == 0) {
		printf(" | MAR=%04x | Data? ", SOL1_REGISTERS::value(sol1_cpu.registers.MARl, sol1_cpu.registers.MARh));
		input = gets(4);

		if (strlen(input) == 0) { printf("\n");  return; }

		leftpad(input, value, 4);
		SOL1_MWORD index = convert_hexstr_to_value(value);
		SOL1_REGISTERS::set(sol1_cpu.registers.MARl, sol1_cpu.registers.MARh, index);
		SOL1_REGISTERS::set(sol1_cpu.registers.MDRl, sol1_cpu.registers.MDRh, sol1_cpu.memory.memory[index]);
	}

	else if (strcmp(input, "MDR") == 0) {
		printf(" | MDR=%04x | Data? ", SOL1_REGISTERS::value(sol1_cpu.registers.MDRl, sol1_cpu.registers.MDRh));
		input = gets(4);

		if (strlen(input) == 0) { printf("\n");  return; }

		leftpad(input, value, 4);
		SOL1_MWORD index = convert_hexstr_to_value(value);
		SOL1_REGISTERS::set(sol1_cpu.registers.MDRl, sol1_cpu.registers.MDRh, index);
	}
	printf("\n");
}

void debugmenu_main_edit_breakpoint(SOL1_CPU& sol1_cpu) {

	char *input;
	char *value = (char*)malloc(sizeof(char) * 257);

	printf("Edit Breakpoint | BKPT=%04x | Address (FFFF=disable) ? ", sol1_cpu.BKPT);
	input = gets(4);

	if (strlen(input) == 0) { printf("\n");  return; }

	leftpad(input, value, 4);
	sol1_cpu.BKPT = convert_hexstr_to_value(value);

	printf("\n");
}

void debugmenu_main_edit_programcounter(SOL1_CPU& sol1_cpu) {

	char *input;
	char *value = (char*)malloc(sizeof(char) * 257);

	printf("Edit Program Counter | PC=%04x | Address ? ", SOL1_REGISTERS::value(sol1_cpu.registers.PCl, sol1_cpu.registers.PCh));
	input = gets(4);

	if (strlen(input) == 0) { printf("\n");  return; }

	leftpad(input, value, 4);
	SOL1_REGISTERS::set(sol1_cpu.registers.PCl, sol1_cpu.registers.PCh, convert_hexstr_to_value(value));
	printf("\n");
}

void debugmenu_main_reset_cpu(SOL1_CPU& sol1_cpu) {


	printf("Reset CPU\n");
	printf("\n");
	sol1_cpu.microcode.init();
	sol1_cpu.reset();
	sol1_cpu.display_registers();
	
}


//debugmenu_main_menu();
int debugmenu_main(SOL1_CPU& sol1_cpu) {

	while (1) {

		printf("> ");
		int key = getch();

		////////

		if (key == (int)'d' || key == (int)'D')
			sol1_cpu.memory.display(sol1_cpu.registers);

		//else if (key == (int)'i' || key == (int)'I')

		else if (key == (int)'e' || key == (int)'E')
			debugmenu_main_edit_mem(sol1_cpu.memory);

		else if (key == (int)'f' || key == (int)'F')
			debugmenu_main_fill_mem(sol1_cpu.memory);

		else if (key == (int)'l' || key == (int)'L')
			debugmenu_main_load_mem(sol1_cpu);

		////////

		else if (key == (int)'r' || key == (int)'R')
			debugmenu_main_display_registers(sol1_cpu);

		else if (key == (int)'a' || key == (int)'A')
			debugmenu_main_edit_register(sol1_cpu);

		else if (key == (int)'b' || key == (int)'B')
			debugmenu_main_edit_breakpoint(sol1_cpu);

		else if (key == (int)'p' || key == (int)'P')
			debugmenu_main_edit_programcounter(sol1_cpu);

		////////

		else if (key == (int)'g' || key == (int)'G') {
			return 1;
		}

		else if (key == (int)'t' || key == (int)'T') {
			return 2;
		}

		else if (key == (int)'z' || key == (int)'Z')
			debugmenu_main_reset_cpu(sol1_cpu);

		////////

		else if (key == (int)'x' || key == (int)'X')
			debugmenu_roms(sol1_cpu);

		////////

		else if (key == (int)'?')
			debugmenu_main_menu();

		else if (key == (int)'q' || key == (int)'Q')
			return 0;

		//----------------------
		else if (key == (int)'m' || key == (int)'M') {
			sol1_cpu.memory.debug_manual_offset = sol1_cpu.memory.debug_manual_offset == 0x00 ? 0x01 : 0x00;

			if (!check_byte_bit(sol1_cpu.registers.MSWl.value(), MSW_PAGING_EN))
				sol1_cpu.memory.display(sol1_cpu.registers);
			else
				sol1_cpu.memory.display_test(sol1_cpu.registers);
		}

		else if (key == (int)'2') {
			if (!check_byte_bit(sol1_cpu.registers.MSWl.value(), MSW_PAGING_EN)) {
				if (sol1_cpu.memory.debug_mem_offset + 0x10 < SOL1_BIOS_MEMORY_SIZE)
					sol1_cpu.memory.debug_mem_offset += 0x10;
				sol1_cpu.memory.debug_manual_offset = 1;
				
				sol1_cpu.memory.display(sol1_cpu.registers);

			}
			else {
				if (sol1_cpu.memory.debug_mem_offset + 0x10 < SOL1_MAIN_MEMORY_SIZE)
					sol1_cpu.memory.debug_mem_offset += 0x10;
				sol1_cpu.memory.debug_manual_offset = 1;

				sol1_cpu.memory.display_test(sol1_cpu.registers);
			}
		}
		if (key == (int)'1') {
			if (sol1_cpu.memory.debug_mem_offset - 0x10 >= 0)
				sol1_cpu.memory.debug_mem_offset -= 0x10;
			else
				sol1_cpu.memory.debug_mem_offset = 0;
			sol1_cpu.memory.debug_manual_offset = 1;

			if (!check_byte_bit(sol1_cpu.registers.MSWl.value(), MSW_PAGING_EN))
				sol1_cpu.memory.display(sol1_cpu.registers);
			else
				sol1_cpu.memory.display_test(sol1_cpu.registers);
		}
		if (key == (int)'4') {
			if (!check_byte_bit(sol1_cpu.registers.MSWl.value(), MSW_PAGING_EN)) {
				if (sol1_cpu.memory.debug_mem_offset + 0x100 < SOL1_BIOS_MEMORY_SIZE)
					sol1_cpu.memory.debug_mem_offset += 0x100;
				sol1_cpu.memory.debug_manual_offset = 1;

				sol1_cpu.memory.display(sol1_cpu.registers);
			}
			else {
				if (sol1_cpu.memory.debug_mem_offset + 0x100 < SOL1_MAIN_MEMORY_SIZE)
					sol1_cpu.memory.debug_mem_offset += 0x100;
				sol1_cpu.memory.debug_manual_offset = 1;


				sol1_cpu.memory.display_test(sol1_cpu.registers);
			}
		}
		if (key == (int)'3') {
			if (sol1_cpu.memory.debug_mem_offset - 0x100 >= 0)
				sol1_cpu.memory.debug_mem_offset -= 0x100;
			else
				sol1_cpu.memory.debug_mem_offset = 0;
			sol1_cpu.memory.debug_manual_offset = 1;

			if (!check_byte_bit(sol1_cpu.registers.MSWl.value(), MSW_PAGING_EN))
				sol1_cpu.memory.display(sol1_cpu.registers);
			else
				sol1_cpu.memory.display_test(sol1_cpu.registers);
		}
		if (key == (int)'6') {
			if (!check_byte_bit(sol1_cpu.registers.MSWl.value(), MSW_PAGING_EN)) {
				if (sol1_cpu.memory.debug_mem_offset + 0x1000 < SOL1_BIOS_MEMORY_SIZE)
					sol1_cpu.memory.debug_mem_offset += 0x1000;
				sol1_cpu.memory.debug_manual_offset = 1;

				sol1_cpu.memory.display(sol1_cpu.registers);
			}
			else {
				if (sol1_cpu.memory.debug_mem_offset + 0x1000 < SOL1_MAIN_MEMORY_SIZE)
					sol1_cpu.memory.debug_mem_offset += 0x1000;
				sol1_cpu.memory.debug_manual_offset = 1;
				sol1_cpu.memory.display_test(sol1_cpu.registers);
			}
		}
		if (key == (int)'5') {
			if (sol1_cpu.memory.debug_mem_offset - 0x1000 >= 0)
				sol1_cpu.memory.debug_mem_offset -= 0x1000;
			else
				sol1_cpu.memory.debug_mem_offset = 0;
			sol1_cpu.memory.debug_manual_offset = 1;

			if (!check_byte_bit(sol1_cpu.registers.MSWl.value(), MSW_PAGING_EN))
				sol1_cpu.memory.display(sol1_cpu.registers);
			else
				sol1_cpu.memory.display_test(sol1_cpu.registers);

		}



		else if (key == (int)'@') {
			if (sol1_cpu.memory.debug_mem_offset + 0x10 < SOL1_BIOS_MEMORY_SIZE)
				sol1_cpu.memory.debug_mem_offset += 0x10;
			sol1_cpu.memory.debug_manual_offset = 1;

			sol1_cpu.memory.display(sol1_cpu.registers);
		}
		if (key == (int)'!') {
			if (sol1_cpu.memory.debug_mem_offset - 0x10 >= 0)
				sol1_cpu.memory.debug_mem_offset -= 0x10;
			else
				sol1_cpu.memory.debug_mem_offset = 0;
			sol1_cpu.memory.debug_manual_offset = 1;

			sol1_cpu.memory.display(sol1_cpu.registers);
		}
		if (key == (int)'$') {
			if (sol1_cpu.memory.debug_mem_offset + 0x100 < SOL1_BIOS_MEMORY_SIZE)
				sol1_cpu.memory.debug_mem_offset += 0x100;
			sol1_cpu.memory.debug_manual_offset = 1;

			sol1_cpu.memory.display(sol1_cpu.registers);
		}
		if (key == (int)'#') {
			if (sol1_cpu.memory.debug_mem_offset - 0x100 >= 0)
				sol1_cpu.memory.debug_mem_offset -= 0x100;
			else
				sol1_cpu.memory.debug_mem_offset = 0;
			sol1_cpu.memory.debug_manual_offset = 1;

			sol1_cpu.memory.display(sol1_cpu.registers);
		}
		if (key == (int)'^') {
			if (sol1_cpu.memory.debug_mem_offset + 0x1000 < SOL1_BIOS_MEMORY_SIZE)
				sol1_cpu.memory.debug_mem_offset += 0x1000;
			sol1_cpu.memory.debug_manual_offset = 1;

			sol1_cpu.memory.display(sol1_cpu.registers);
		}
		if (key == (int)'%') {
			if (sol1_cpu.memory.debug_mem_offset - 0x1000 >= 0)
				sol1_cpu.memory.debug_mem_offset -= 0x1000;
			else
				sol1_cpu.memory.debug_mem_offset = 0;
			sol1_cpu.memory.debug_manual_offset = 1;

			sol1_cpu.memory.display(sol1_cpu.registers);

		}

		//----------------------
		////////

		else
			printf("\n");

		////////
	}

	return 0;
}

