#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include <ctype.h>

#include <chrono>
#ifdef __MINGW32__
#include <time.h>
#include <pthread.h> 
#endif

#include "config.h"
#include "debugmenu_main.h"
#include "sol1_alu.h"
#include "sol1_cpu.h"


using namespace std::chrono;



int main(int argc, char** argv) {

	sol1_alu alu;
	/*
	void sol1_alu_xor(struct sol1_alu* alu);
	void sol1_alu_or(struct sol1_alu* alu);
	void sol1_alu_and(struct sol1_alu* alu);
	void sol1_alu_not(struct sol1_alu* alu);
	void sol1_alu_shl(struct sol1_alu* alu);
	void sol1_alu_shr(struct sol1_alu* alu);

	void sol1_alu_sum(struct sol1_alu* alu);
	void sol1_alu_sub(struct sol1_alu* alu);


	alu.A = 0xFFFF;// 0b0000000000000000;
	alu.B = 0xFFFF;// 0b0000000000000000;
	alu.C = 0x0;
	alu.F = 0x0;
	alu.CIN = 0x0;

	sol1_alu_xor(&alu);

	printf("A: ");
	print_word_bin(alu.A);

	printf("B: ");
	print_word_bin(alu.B);
	printf("\n");

	printf("C: ");
	print_word_bin(alu.C);
	printf("\n");

	printf("Flags: ");
	print_byte_bin(alu.F);
	printf("\n");


	if (alu.F & 0x01 == 0x01)
		printf("SOL1_ALU_ZERO\n");
	if ((alu.F >> 1) & 0x01 == 0x01)
		printf("SOL1_ALU_EQUAL\n");
	if ((alu.F >> 2) & 0x01 == 0x01)
		printf("SOL1_ALU_LARGER\n");
	if ((alu.F >> 3) & 0x01 == 0x01)
		printf("SOL1_ALU_CARRY_OUT\n");
*/

	sol1_cpu sol1_cpu;
	sol1_cpu_init(&sol1_cpu);

	debugmenu_main_menu();

	while (1) {

		printf("> ");
		int key = getch();

		////////

		if (key == (int)'d' || key == (int)'D')
			sol1_display_memory(&sol1_cpu.memory);

		//else if (key == (int)'i' || key == (int)'I')

		else if (key == (int)'e' || key == (int)'E')
			debugmenu_main_edit_mem(&sol1_cpu.memory);

		else if (key == (int)'f' || key == (int)'F')
			debugmenu_main_fill_mem(&sol1_cpu.memory);

		else if (key == (int)'l' || key == (int)'L')
			debugmenu_main_load_mem(&sol1_cpu);

		////////

		else if (key == (int)'r' || key == (int)'R')
			debugmenu_main_display_registers(&sol1_cpu);

		else if (key == (int)'a' || key == (int)'A')
			debugmenu_main_edit_register(&sol1_cpu);

		else if (key == (int)'b' || key == (int)'B')
			debugmenu_main_edit_breakpoint(&sol1_cpu);

		else if (key == (int)'p' || key == (int)'P')
			debugmenu_main_edit_programcounter(&sol1_cpu);

		////////

		//else if (key == (int)'g' || key == (int)'G')

		//else if (key == (int)'s' || key == (int)'S')

		else if (key == (int)'z' || key == (int)'Z')
			debugmenu_main_reset_cpu(&sol1_cpu);

		////////

		else if (key == (int)'?')
			debugmenu_main_menu();

		else if (key == (int)'q' || key == (int)'Q')
			exit(0);

		////////

		else
			printf("\n");

		////////
	}

	return 0;
}
