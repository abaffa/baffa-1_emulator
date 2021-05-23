#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "debugmenu_roms.h"
#include "sol1_rom.h"
#include "utils.h"

void debugmenu_roms_menu() {
	printf("\n");
	printf("SOL-1 Debug Monitor > Roms\n");
	printf("\n");
	printf("  C - Display Rom Microcode Cycles\n");
	//printf("  I - Disassemble Memory\n");
	//printf("  E - Edit Memory\n");
	//printf("  F - Fill Memory\n");
	//printf("  L - Load Memory\n");
	//printf("\n");
	//printf("  R - Display Registers\n");
	//printf("  A - Edit Register\n");
	printf("  B - Edit Breakpoint\n");
	//printf("  P - Edit Program Counter\n");
	//printf("\n");
	//printf("  G - Go(Run)\n");
	//printf("  S - Step\n");
	//printf("  Z - Reset CPU\n");
	//printf("  Z - Reset CPU\n");
	//printf("\n");
	printf("  ? - Display Menu\n");
	printf("  Q - Back to Debug Monitor\n");
	printf("\n");
}

void debugmenu_roms_edit_breakpoint(struct sol1_rom *sol1_rom) {

	char *input;
	char *value = (char*)malloc(sizeof(char) * 257);

	printf("Edit Breakpoint (Opcode/Cycle:%02x%02x | 0000=disable) | Opcode ? ", sol1_rom->bkpt_opcode, sol1_rom->bkpt_cycle);
	input = gets(2);

	if (strlen(input) == 0) { printf("\n");  return; }

	leftpad(input, value, 2);
	sol1_rom->bkpt_opcode = convert_hexstr_to_value(value);

	printf(" | Cycle ? ");
	input = gets(2);

	if (strlen(input) == 0) { printf("\n");  return; }

	leftpad(input, value, 2);
	sol1_rom->bkpt_cycle = convert_hexstr_to_value(value);


	printf("\n");
}


void debugmenu_roms(struct sol1_cpu *sol1_cpu) {
	debugmenu_roms_menu();

	while (1) {

		printf("roms> ");
		int key = getch();

		////////
		
		if (key == (int)'c' || key == (int)'C')
		{
			sol1_rom_debug_cycles(&sol1_cpu->rom);
		}//sol1_memory_display(&sol1_cpu->memory);
		/*
		//else if (key == (int)'i' || key == (int)'I')

		else if (key == (int)'e' || key == (int)'E')
			debugmenu_main_edit_mem(&sol1_cpu->memory);

		else if (key == (int)'f' || key == (int)'F')
			debugmenu_main_fill_mem(&sol1_cpu->memory);

		else if (key == (int)'l' || key == (int)'L')
			debugmenu_main_load_mem(sol1_cpu);

		////////

		else if (key == (int)'r' || key == (int)'R')
			debugmenu_main_display_registers(sol1_cpu);

		else if (key == (int)'a' || key == (int)'A')
			debugmenu_main_edit_register(sol1_cpu);
			*/
		else if (key == (int)'b' || key == (int)'B')
			debugmenu_roms_edit_breakpoint(&sol1_cpu->rom);
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
			debugmenu_roms_menu();

		else if (key == (int)'q' || key == (int)'Q')
			break;

		////////

		else
			printf("\n");

		////////
	}

	printf("\n");
}