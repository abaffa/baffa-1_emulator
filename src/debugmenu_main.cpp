#include <stdio.h>

#if defined(__linux__) || defined(__MINGW32__)

#else

#endif

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "debugmenu_main.h"
#include "debugmenu_roms.h"
#include "utils.h"
#include "hw_tty.h"

void debugmenu_main_menu(HW_TTY& hw_tty) {
	hw_tty.print("\n");
	hw_tty.print("SOL-1 Debug Monitor | 30 May 2021\n");
	hw_tty.print("\n");
	hw_tty.print("  D - Display Memory\n");
	hw_tty.print("  I - Disassemble Memory\n");
	hw_tty.print("  E - Edit Memory\n");
	hw_tty.print("  F - Fill Memory\n");
	hw_tty.print("  L - Load Memory\n");
	hw_tty.print("\n");
	hw_tty.print("  R - Display Registers\n");
	hw_tty.print("  A - Edit Register\n");
	hw_tty.print("  B - Edit Breakpoint\n");
	hw_tty.print("  P - Edit Program Counter\n");
	hw_tty.print("\n");
	hw_tty.print("  G - Go(Run)\n");
	hw_tty.print("  T - Debug Trace\n");
	hw_tty.print("  Z - Reset CPU\n");
	hw_tty.print("  V - Reload Bios\n");
	hw_tty.print("\n");
	hw_tty.print("  X - Debug Roms\n");
	hw_tty.print("\n");
	hw_tty.print("  ? - Display Menu\n");
	hw_tty.print("  Q - Quit\n");
	hw_tty.print("\n");
}


void debugmenu_main_disassemble_mem(HW_TTY& hw_tty) {
}


void debugmenu_main_edit_mem(SOL1_MEMORY& memory, HW_TTY& hw_tty) {

	char str_out[255];
	char *input;
	char *value = (char*)malloc(sizeof(char) * 257);

	hw_tty.print("Edit Memory | Address? ");
	input = hw_tty.gets(4);

	if (strlen(input) == 0) { hw_tty.print("\n");  return; }

	leftpad(input, value, 4);

	unsigned short address = convert_hexstr_to_value(value);
	sprintf(str_out, "\n  %04x=%02x ? ", address, memory.memory[address]); hw_tty.print(str_out);
	input = hw_tty.gets(2);

	while (strlen(input) > 0) {

		leftpad(input, value, 2);

		unsigned char b = convert_hexstr_to_value(value);

		memory.memory[address] = b;
		address++;
		if (address >= SOL1_BIOS_MEMORY_SIZE) { hw_tty.print("\n");  return; }

		sprintf(str_out, "\n  %04x=%02x ? ", address, memory.memory[address]); hw_tty.print(str_out);
		input = hw_tty.gets(2);
	}

	hw_tty.print("\n");
}



void debugmenu_main_fill_mem(SOL1_MEMORY& memory, HW_TTY& hw_tty) {

	char *input;
	char *value = (char*)malloc(sizeof(char) * 257);
	unsigned long address, start, end;
	unsigned char data;

	hw_tty.print("Fill Memory | Start Address? ");
	input = hw_tty.gets(4);

	if (strlen(input) == 0) { hw_tty.print("\n");  return; }

	leftpad(input, value, 4);
	start = convert_hexstr_to_value(value);

	hw_tty.print(" | End Address? ");
	input = hw_tty.gets(4);

	if (strlen(input) == 0) { hw_tty.print("\n");  return; }

	leftpad(input, value, 4);
	end = convert_hexstr_to_value(value);

	hw_tty.print(" | Data? ");
	input = hw_tty.gets(2);

	if (strlen(input) == 0) { hw_tty.print("\n");  return; }

	leftpad(input, value, 2);
	data = convert_hexstr_to_value(value);


	for (address = start; address <= end && address < SOL1_BIOS_MEMORY_SIZE; address++)
		memory.memory[address] = data;

	hw_tty.print("\n");
}




void debugmenu_main_load_mem(SOL1_CPU& sol1_cpu, HW_TTY& hw_tty) {

	char *input;
	char *value = (char*)malloc(sizeof(char) * 257);
	unsigned short start; //address,  end
//	unsigned char data;

	hw_tty.print("Load Memory | Start Address? ");
	input = hw_tty.gets(4);

	if (strlen(input) == 0) { hw_tty.print("\n");  return; }

	leftpad(input, value, 4);
	start = convert_hexstr_to_value(value);


	hw_tty.print("\n  Code (max 128 bytes) ] ");
	input = hw_tty.gets(256);

	if (strlen(input) == 0) { hw_tty.print("\n");  return; }

	size_t numdigits = strlen(input) / 2;
	size_t i;
	for (i = 0; i != numdigits; ++i)
	{
		unsigned char output = 16 * toInt(input[2 * i]) + toInt(input[2 * i + 1]);
		sol1_cpu.memory.memory[i + start] = output;
	}

	SOL1_REGISTERS::set(sol1_cpu.registers.PCl, sol1_cpu.registers.PCh, start);

	hw_tty.print("\n");
}



void debugmenu_main_display_registers(SOL1_CPU& sol1_cpu, HW_TTY& hw_tty) {

	hw_tty.print("Display Registers\n");
	hw_tty.print("\n");
	sol1_cpu.display_registers(hw_tty);
}

void debugmenu_main_edit_register(SOL1_CPU& sol1_cpu, HW_TTY& hw_tty) {

	char str_out[255];
	char *input;
	char *value = (char*)malloc(sizeof(char) * 257);
	//unsigned short address; // , end
//	unsigned char data;

	hw_tty.print("Edit Register ? ");
	input = hw_tty.gets(3);

	if (strlen(input) == 0) { hw_tty.print("\n");  return; }

	if (strcmp(input, "A") == 0) {
		sprintf(str_out, " | A=%04x | Data? ", SOL1_REGISTERS::value(sol1_cpu.registers.Al, sol1_cpu.registers.Ah)); hw_tty.print(str_out);
		input = hw_tty.gets(4);

		if (strlen(input) == 0) { hw_tty.print("\n");  return; }

		leftpad(input, value, 4);
		SOL1_REGISTERS::set(sol1_cpu.registers.Al, sol1_cpu.registers.Ah, convert_hexstr_to_value(value));
	}
	else if (strcmp(input, "B") == 0) {
		sprintf(str_out, " | B=%04x | Data? ", SOL1_REGISTERS::value(sol1_cpu.registers.Bl, sol1_cpu.registers.Bh)); hw_tty.print(str_out);
		input = hw_tty.gets(4);

		if (strlen(input) == 0) { hw_tty.print("\n");  return; }

		leftpad(input, value, 4);
		SOL1_REGISTERS::set(sol1_cpu.registers.Bl, sol1_cpu.registers.Bh, convert_hexstr_to_value(value));
	}
	else if (strcmp(input, "C") == 0) {
		sprintf(str_out, " | C=%04x | Data? ", SOL1_REGISTERS::value(sol1_cpu.registers.Cl, sol1_cpu.registers.Ch)); hw_tty.print(str_out);
		input = hw_tty.gets(4);

		if (strlen(input) == 0) { hw_tty.print("\n");  return; }

		leftpad(input, value, 4);
		SOL1_REGISTERS::set(sol1_cpu.registers.Cl, sol1_cpu.registers.Ch, convert_hexstr_to_value(value));
	}
	else if (strcmp(input, "D") == 0) {
		sprintf(str_out, " | D=%04x | Data? ", SOL1_REGISTERS::value(sol1_cpu.registers.Dl, sol1_cpu.registers.Dh)); hw_tty.print(str_out);
		input = hw_tty.gets(4);

		if (strlen(input) == 0) { hw_tty.print("\n");  return; }

		leftpad(input, value, 4);
		SOL1_REGISTERS::set(sol1_cpu.registers.Dl, sol1_cpu.registers.Dh, convert_hexstr_to_value(value));
	}
	else if (strcmp(input, "G") == 0) {
		sprintf(str_out, " | G=%04x | Data? ", SOL1_REGISTERS::value(sol1_cpu.registers.Gl, sol1_cpu.registers.Gh)); hw_tty.print(str_out);
		input = hw_tty.gets(4);

		if (strlen(input) == 0) { hw_tty.print("\n");  return; }

		leftpad(input, value, 4);
		SOL1_REGISTERS::set(sol1_cpu.registers.Gl, sol1_cpu.registers.Gh, convert_hexstr_to_value(value));
	}


	else if (strcmp(input, "BP") == 0) {
		sprintf(str_out, " | BP=%04x | Data? ", SOL1_REGISTERS::value(sol1_cpu.registers.BPl, sol1_cpu.registers.BPh)); hw_tty.print(str_out);
		input = hw_tty.gets(4);

		if (strlen(input) == 0) { hw_tty.print("\n");  return; }

		leftpad(input, value, 4);
		SOL1_REGISTERS::set(sol1_cpu.registers.BPl, sol1_cpu.registers.BPh, convert_hexstr_to_value(value));
	}
	else if (strcmp(input, "SP") == 0) {
		sprintf(str_out, " | SP=%04x | Data? ", SOL1_REGISTERS::value(sol1_cpu.registers.SPl, sol1_cpu.registers.SPh)); hw_tty.print(str_out);
		input = hw_tty.gets(4);

		if (strlen(input) == 0) { hw_tty.print("\n");  return; }

		leftpad(input, value, 4);
		SOL1_REGISTERS::set(sol1_cpu.registers.SPl, sol1_cpu.registers.SPh, convert_hexstr_to_value(value));
	}

	else if (strcmp(input, "SI") == 0) {
		sprintf(str_out, " | SI=%04x | Data? ", SOL1_REGISTERS::value(sol1_cpu.registers.SIl, sol1_cpu.registers.SIh)); hw_tty.print(str_out);
		input = hw_tty.gets(4);

		if (strlen(input) == 0) { hw_tty.print("\n");  return; }

		leftpad(input, value, 4);
		SOL1_REGISTERS::set(sol1_cpu.registers.SIl, sol1_cpu.registers.SIh, convert_hexstr_to_value(value));
	}
	else if (strcmp(input, "DI") == 0) {
		sprintf(str_out, " | DI=%04x | Data? ", SOL1_REGISTERS::value(sol1_cpu.registers.DIl, sol1_cpu.registers.DIh)); hw_tty.print(str_out);
		input = hw_tty.gets(4);

		if (strlen(input) == 0) { hw_tty.print("\n");  return; }

		leftpad(input, value, 4);
		SOL1_REGISTERS::set(sol1_cpu.registers.DIl, sol1_cpu.registers.DIh, convert_hexstr_to_value(value));
	}
	else if (strcmp(input, "PC") == 0) {
		sprintf(str_out, " | PC=%04x | Data? ", SOL1_REGISTERS::value(sol1_cpu.registers.PCl, sol1_cpu.registers.PCh)); hw_tty.print(str_out);
		input = hw_tty.gets(4);

		if (strlen(input) == 0) { hw_tty.print("\n");  return; }

		leftpad(input, value, 4);
		SOL1_REGISTERS::set(sol1_cpu.registers.PCl, sol1_cpu.registers.PCh, convert_hexstr_to_value(value));
	}


	else if (strcmp(input, "TDR") == 0) {
		sprintf(str_out, " | TDR=%04x | Data? ", SOL1_REGISTERS::value(sol1_cpu.registers.TDRl, sol1_cpu.registers.TDRh)); hw_tty.print(str_out);
		input = hw_tty.gets(4);

		if (strlen(input) == 0) { hw_tty.print("\n");  return; }

		leftpad(input, value, 4);
		SOL1_REGISTERS::set(sol1_cpu.registers.TDRl, sol1_cpu.registers.TDRh, convert_hexstr_to_value(value));
	}
	else if (strcmp(input, "SSP") == 0) {
		sprintf(str_out, " | SSP=%04x | Data? ", SOL1_REGISTERS::value(sol1_cpu.registers.SSPl, sol1_cpu.registers.SSPh)); hw_tty.print(str_out);
		input = hw_tty.gets(4);

		if (strlen(input) == 0) { hw_tty.print("\n");  return; }

		leftpad(input, value, 4);
		SOL1_REGISTERS::set(sol1_cpu.registers.SSPl, sol1_cpu.registers.SSPh, convert_hexstr_to_value(value));

	}
	else if (strcmp(input, "PTB") == 0) {
		sprintf(str_out, " | PTB=%02x | Data? ", sol1_cpu.registers.PTB.value());
		input = hw_tty.gets(2);

		if (strlen(input) == 0) { hw_tty.print("\n");  return; }

		leftpad(input, value, 2);
		sol1_cpu.registers.PTB.set(convert_hexstr_to_value(value));
	}
	else if (strcmp(input, "MSW") == 0) {
		sprintf(str_out, " | MSW=%04x | Data? ", SOL1_REGISTERS::value(sol1_cpu.registers.MSWl, sol1_cpu.registers.MSWh)); hw_tty.print(str_out);
		input = hw_tty.gets(4);

		if (strlen(input) == 0) { hw_tty.print("\n");  return; }

		leftpad(input, value, 4);
		SOL1_REGISTERS::set(sol1_cpu.registers.MSWl, sol1_cpu.registers.MSWh, convert_hexstr_to_value(value));
	}

	else if (strcmp(input, "MAR") == 0) {
		sprintf(str_out, " | MAR=%04x | Data? ", SOL1_REGISTERS::value(sol1_cpu.registers.MARl, sol1_cpu.registers.MARh)); hw_tty.print(str_out);
		input = hw_tty.gets(4);

		if (strlen(input) == 0) { hw_tty.print("\n");  return; }

		leftpad(input, value, 4);
		SOL1_MWORD index = convert_hexstr_to_value(value);
		SOL1_REGISTERS::set(sol1_cpu.registers.MARl, sol1_cpu.registers.MARh, index);
		SOL1_REGISTERS::set(sol1_cpu.registers.MDRl, sol1_cpu.registers.MDRh, sol1_cpu.memory.memory[index]);
	}

	else if (strcmp(input, "MDR") == 0) {
		sprintf(str_out, " | MDR=%04x | Data? ", SOL1_REGISTERS::value(sol1_cpu.registers.MDRl, sol1_cpu.registers.MDRh)); hw_tty.print(str_out);
		input = hw_tty.gets(4);

		if (strlen(input) == 0) { hw_tty.print("\n");  return; }

		leftpad(input, value, 4);
		SOL1_MWORD index = convert_hexstr_to_value(value);
		SOL1_REGISTERS::set(sol1_cpu.registers.MDRl, sol1_cpu.registers.MDRh, index);
	}
	hw_tty.print("\n");
}

void debugmenu_main_edit_breakpoint(SOL1_CPU& sol1_cpu, HW_TTY& hw_tty) {
	char str_out[255];
	char *input;
	char *value = (char*)malloc(sizeof(char) * 257);

	sprintf(str_out, "Edit Breakpoint | BKPT=%04x | Address (FFFF=disable) ? ", sol1_cpu.BKPT); hw_tty.print(str_out);
	input = hw_tty.gets(4);

	if (strlen(input) == 0) { hw_tty.print("\n");  return; }

	leftpad(input, value, 4);
	sol1_cpu.BKPT = convert_hexstr_to_value(value);

	hw_tty.print("\n");
}

void debugmenu_main_edit_programcounter(SOL1_CPU& sol1_cpu, HW_TTY& hw_tty) {

	char str_out[255];
	char *input;
	char *value = (char*)malloc(sizeof(char) * 257);

	sprintf(str_out, "Edit Program Counter | PC=%04x | Address ? ", SOL1_REGISTERS::value(sol1_cpu.registers.PCl, sol1_cpu.registers.PCh)); hw_tty.print(str_out);
	input = hw_tty.gets(4);

	if (strlen(input) == 0) { hw_tty.print("\n");  return; }

	leftpad(input, value, 4);
	SOL1_REGISTERS::set(sol1_cpu.registers.PCl, sol1_cpu.registers.PCh, convert_hexstr_to_value(value));
	hw_tty.print("\n");
}

void debugmenu_main_reset_cpu(SOL1_CPU& sol1_cpu, HW_TTY& hw_tty) {

	hw_tty.print("Reset CPU\n");
	hw_tty.print("\n");
	sol1_cpu.microcode.init();
	sol1_cpu.reset();
	sol1_cpu.display_registers(hw_tty);
}

void debugmenu_main_reload_bios(SOL1_CPU& sol1_cpu, HW_TTY& hw_tty) {

	hw_tty.print("Reloading BIOS\n");
	sol1_cpu.memory.load_bios();
}


//debugmenu_main_menu();
int debugmenu_main(SOL1_CPU& sol1_cpu, HW_TTY& hw_tty) {

	while (1) {

		hw_tty.print("> ");
		int key = hw_tty.get_char();

		////////

		if (key == (int)'d' || key == (int)'D')
			sol1_cpu.memory.display(sol1_cpu.registers, hw_tty);

		//else if (key == (int)'i' || key == (int)'I')

		else if (key == (int)'e' || key == (int)'E')
			debugmenu_main_edit_mem(sol1_cpu.memory, hw_tty);

		else if (key == (int)'f' || key == (int)'F')
			debugmenu_main_fill_mem(sol1_cpu.memory, hw_tty);

		else if (key == (int)'l' || key == (int)'L')
			debugmenu_main_load_mem(sol1_cpu, hw_tty);

		////////

		else if (key == (int)'r' || key == (int)'R')
			debugmenu_main_display_registers(sol1_cpu, hw_tty);

		else if (key == (int)'a' || key == (int)'A')
			debugmenu_main_edit_register(sol1_cpu, hw_tty);

		else if (key == (int)'b' || key == (int)'B')
			debugmenu_main_edit_breakpoint(sol1_cpu, hw_tty);

		else if (key == (int)'p' || key == (int)'P')
			debugmenu_main_edit_programcounter(sol1_cpu, hw_tty);

		////////

		else if (key == (int)'g' || key == (int)'G') {
			return 1;
		}

		else if (key == (int)'t' || key == (int)'T') {
			return 2;
		}

		else if (key == (int)'z' || key == (int)'Z')
			debugmenu_main_reset_cpu(sol1_cpu, hw_tty);

		else if (key == (int)'v' || key == (int)'V')
			debugmenu_main_reload_bios(sol1_cpu, hw_tty);

		////////

		else if (key == (int)'x' || key == (int)'X')
			debugmenu_roms(sol1_cpu, hw_tty);

		////////

		else if (key == (int)'?')
			debugmenu_main_menu(hw_tty);

		else if (key == (int)'q' || key == (int)'Q')
			return 0;

		//----------------------
		else if (key == (int)'m' || key == (int)'M') {
			sol1_cpu.memory.debug_manual_offset = sol1_cpu.memory.debug_manual_offset == 0x00 ? 0x01 : 0x00;

			if (!check_byte_bit(sol1_cpu.registers.MSWl.value(), MSWl_PAGING_EN))
				sol1_cpu.memory.display(sol1_cpu.registers, hw_tty);
			else
				sol1_cpu.memory.display_test(sol1_cpu.registers, hw_tty);
		}

		else if (key == (int)'2') {
			if (!check_byte_bit(sol1_cpu.registers.MSWl.value(), MSWl_PAGING_EN)) {
				if (sol1_cpu.memory.debug_mem_offset + 0x10 < SOL1_BIOS_MEMORY_SIZE)
					sol1_cpu.memory.debug_mem_offset += 0x10;
				sol1_cpu.memory.debug_manual_offset = 1;
				
				sol1_cpu.memory.display(sol1_cpu.registers, hw_tty);

			}
			else {
				if (sol1_cpu.memory.debug_mem_offset + 0x10 < SOL1_MAIN_MEMORY_SIZE)
					sol1_cpu.memory.debug_mem_offset += 0x10;
				sol1_cpu.memory.debug_manual_offset = 1;

				sol1_cpu.memory.display_test(sol1_cpu.registers, hw_tty);
			}
		}
		if (key == (int)'1') {
			if (sol1_cpu.memory.debug_mem_offset - 0x10 >= 0)
				sol1_cpu.memory.debug_mem_offset -= 0x10;
			else
				sol1_cpu.memory.debug_mem_offset = 0;
			sol1_cpu.memory.debug_manual_offset = 1;

			if (!check_byte_bit(sol1_cpu.registers.MSWl.value(), MSWl_PAGING_EN))
				sol1_cpu.memory.display(sol1_cpu.registers, hw_tty);
			else
				sol1_cpu.memory.display_test(sol1_cpu.registers, hw_tty);
		}
		if (key == (int)'4') {
			if (!check_byte_bit(sol1_cpu.registers.MSWl.value(), MSWl_PAGING_EN)) {
				if (sol1_cpu.memory.debug_mem_offset + 0x100 < SOL1_BIOS_MEMORY_SIZE)
					sol1_cpu.memory.debug_mem_offset += 0x100;
				sol1_cpu.memory.debug_manual_offset = 1;

				sol1_cpu.memory.display(sol1_cpu.registers, hw_tty);
			}
			else {
				if (sol1_cpu.memory.debug_mem_offset + 0x100 < SOL1_MAIN_MEMORY_SIZE)
					sol1_cpu.memory.debug_mem_offset += 0x100;
				sol1_cpu.memory.debug_manual_offset = 1;


				sol1_cpu.memory.display_test(sol1_cpu.registers, hw_tty);
			}
		}
		if (key == (int)'3') {
			if (sol1_cpu.memory.debug_mem_offset - 0x100 >= 0)
				sol1_cpu.memory.debug_mem_offset -= 0x100;
			else
				sol1_cpu.memory.debug_mem_offset = 0;
			sol1_cpu.memory.debug_manual_offset = 1;

			if (!check_byte_bit(sol1_cpu.registers.MSWl.value(), MSWl_PAGING_EN))
				sol1_cpu.memory.display(sol1_cpu.registers, hw_tty);
			else
				sol1_cpu.memory.display_test(sol1_cpu.registers, hw_tty);
		}
		if (key == (int)'6') {
			if (!check_byte_bit(sol1_cpu.registers.MSWl.value(), MSWl_PAGING_EN)) {
				if (sol1_cpu.memory.debug_mem_offset + 0x1000 < SOL1_BIOS_MEMORY_SIZE)
					sol1_cpu.memory.debug_mem_offset += 0x1000;
				sol1_cpu.memory.debug_manual_offset = 1;

				sol1_cpu.memory.display(sol1_cpu.registers, hw_tty);
			}
			else {
				if (sol1_cpu.memory.debug_mem_offset + 0x1000 < SOL1_MAIN_MEMORY_SIZE)
					sol1_cpu.memory.debug_mem_offset += 0x1000;
				sol1_cpu.memory.debug_manual_offset = 1;
				sol1_cpu.memory.display_test(sol1_cpu.registers, hw_tty);
			}
		}
		if (key == (int)'5') {
			if (sol1_cpu.memory.debug_mem_offset - 0x1000 >= 0)
				sol1_cpu.memory.debug_mem_offset -= 0x1000;
			else
				sol1_cpu.memory.debug_mem_offset = 0;
			sol1_cpu.memory.debug_manual_offset = 1;

			if (!check_byte_bit(sol1_cpu.registers.MSWl.value(), MSWl_PAGING_EN))
				sol1_cpu.memory.display(sol1_cpu.registers, hw_tty);
			else
				sol1_cpu.memory.display_test(sol1_cpu.registers, hw_tty);

		}



		else if (key == (int)'@') {
			if (sol1_cpu.memory.debug_mem_offset + 0x10 < SOL1_BIOS_MEMORY_SIZE)
				sol1_cpu.memory.debug_mem_offset += 0x10;
			sol1_cpu.memory.debug_manual_offset = 1;

			sol1_cpu.memory.display(sol1_cpu.registers, hw_tty);
		}
		if (key == (int)'!') {
			if (sol1_cpu.memory.debug_mem_offset - 0x10 >= 0)
				sol1_cpu.memory.debug_mem_offset -= 0x10;
			else
				sol1_cpu.memory.debug_mem_offset = 0;
			sol1_cpu.memory.debug_manual_offset = 1;

			sol1_cpu.memory.display(sol1_cpu.registers, hw_tty);
		}
		if (key == (int)'$') {
			if (sol1_cpu.memory.debug_mem_offset + 0x100 < SOL1_BIOS_MEMORY_SIZE)
				sol1_cpu.memory.debug_mem_offset += 0x100;
			sol1_cpu.memory.debug_manual_offset = 1;

			sol1_cpu.memory.display(sol1_cpu.registers, hw_tty);
		}
		if (key == (int)'#') {
			if (sol1_cpu.memory.debug_mem_offset - 0x100 >= 0)
				sol1_cpu.memory.debug_mem_offset -= 0x100;
			else
				sol1_cpu.memory.debug_mem_offset = 0;
			sol1_cpu.memory.debug_manual_offset = 1;

			sol1_cpu.memory.display(sol1_cpu.registers, hw_tty);
		}
		if (key == (int)'^') {
			if (sol1_cpu.memory.debug_mem_offset + 0x1000 < SOL1_BIOS_MEMORY_SIZE)
				sol1_cpu.memory.debug_mem_offset += 0x1000;
			sol1_cpu.memory.debug_manual_offset = 1;

			sol1_cpu.memory.display(sol1_cpu.registers, hw_tty);
		}
		if (key == (int)'%') {
			if (sol1_cpu.memory.debug_mem_offset - 0x1000 >= 0)
				sol1_cpu.memory.debug_mem_offset -= 0x1000;
			else
				sol1_cpu.memory.debug_mem_offset = 0;
			sol1_cpu.memory.debug_manual_offset = 1;

			sol1_cpu.memory.display(sol1_cpu.registers, hw_tty);

		}

		//----------------------
		////////

		else
			hw_tty.print("\n");

		////////
	}

	return 0;
}

