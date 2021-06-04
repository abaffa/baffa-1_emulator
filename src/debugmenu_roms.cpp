#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "hw_tty.h"

#if defined(__linux__) || defined(__MINGW32__)

#else

#endif

#include "debugmenu_roms.h"
#include "sol1_rom.h"
#include "utils.h"

void debugmenu_roms_menu(HW_TTY& hw_tty) {
	hw_tty.print("\n");
	hw_tty.print("SOL-1 Debug Monitor > Roms\n");
	hw_tty.print("\n");
	hw_tty.print("  C - Display Rom Microcode Cycles\n");
	//hw_tty.print("  I - Disassemble Memory\n");
	//hw_tty.print("  E - Edit Memory\n");
	//hw_tty.print("  F - Fill Memory\n");
	//hw_tty.print("  L - Load Memory\n");
	//hw_tty.print("\n");
	//hw_tty.print("  R - Display Registers\n");
	//hw_tty.print("  A - Edit Register\n");
	hw_tty.print("  B - Edit Breakpoint\n");
	//hw_tty.print("  P - Edit Program Counter\n");
	//hw_tty.print("\n");
	//hw_tty.print("  G - Go(Run)\n");
	//hw_tty.print("  S - Step\n");
	//hw_tty.print("  Z - Reset CPU\n");
	//hw_tty.print("  Z - Reset CPU\n");
	//hw_tty.print("\n");
	hw_tty.print("  ? - Display Menu\n");
	hw_tty.print("  Q - Back to Debug Monitor\n");
	hw_tty.print("\n");
}

void debugmenu_roms_edit_breakpoint(SOL1_ROM& sol1_rom, HW_TTY& hw_tty) {
	char str_out[255];
	char *input;
	char *value = (char*)malloc(sizeof(char) * 257);

	sprintf(str_out, "Edit Breakpoint (Opcode/Cycle:%02x%02x | 0000=disable) | Opcode ? ", sol1_rom.bkpt_opcode, sol1_rom.bkpt_cycle); hw_tty.print(str_out);
	input = hw_tty.gets(2);

	if (strlen(input) == 0) { hw_tty.print("\n");  return; }

	leftpad(input, value, 2);
	sol1_rom.bkpt_opcode = convert_hexstr_to_value(value);

	hw_tty.print(" | Cycle ? ");
	input = hw_tty.gets(2);

	if (strlen(input) == 0) { hw_tty.print("\n");  return; }

	leftpad(input, value, 2);
	sol1_rom.bkpt_cycle = convert_hexstr_to_value(value);


	hw_tty.print("\n");
}


void debugmenu_roms(SOL1_CPU& sol1_cpu, HW_TTY& hw_tty) {
	debugmenu_roms_menu(hw_tty);

	while (1) {

		hw_tty.print("roms> ");
		int key = hw_tty.get_char();

		////////
		
		if (key == (int)'c' || key == (int)'C')
		{
			sol1_cpu.rom.debug_cycles(hw_tty);
		}//sol1_memory_display(&sol1_cpu.memory);
		/*
		//else if (key == (int)'i' || key == (int)'I')

		else if (key == (int)'e' || key == (int)'E')
			debugmenu_main_edit_mem(&sol1_cpu.memory);

		else if (key == (int)'f' || key == (int)'F')
			debugmenu_main_fill_mem(&sol1_cpu.memory);

		else if (key == (int)'l' || key == (int)'L')
			debugmenu_main_load_mem(sol1_cpu);

		////////

		else if (key == (int)'r' || key == (int)'R')
			debugmenu_main_display_registers(sol1_cpu);

		else if (key == (int)'a' || key == (int)'A')
			debugmenu_main_edit_register(sol1_cpu);
			*/
		else if (key == (int)'b' || key == (int)'B')
			debugmenu_roms_edit_breakpoint(sol1_cpu.rom, hw_tty);
		/*
		else if (key == (int)'p' || key == (int)'P')
			debugmenu_main_edit_programcounter(sol1_cpu);

		////////

		//else if (key == (int)'g' || key == (int)'G')

		//else if (key == (int)'s' || key == (int)'S')

		else if (key == (int)'z' || key == (int)'Z')
			debugmenu_main_reset_cpu(sol1_cpu);

		////////
		*/
		else if (key == (int)'?')
			debugmenu_roms_menu(hw_tty);

		else if (key == (int)'q' || key == (int)'Q')
			break;

		////////

		else
			hw_tty.print("\n");

		////////
	}

	hw_tty.print("\n");
}