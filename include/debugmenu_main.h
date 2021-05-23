#ifndef DEBUGMENU_MAIN_H
#define DEBUGMENU_MAIN_H

#include "sol1_cpu.h"

void debugmenu_main_menu();
void debugmenu_main_disassemble_mem();
void debugmenu_main_edit_mem(struct sol1_memory* memory);
void debugmenu_main_fill_mem(struct sol1_memory* memory);
void debugmenu_main_load_mem(struct sol1_cpu *sol1_cpu);
void debugmenu_main_display_registers(struct sol1_cpu *sol1_cpu);
void debugmenu_main_edit_register(struct sol1_cpu *sol1_cpu);
void debugmenu_main_edit_breakpoint(struct sol1_cpu *sol1_cpu);
void debugmenu_main_edit_programcounter(struct sol1_cpu *sol1_cpu);
void debugmenu_main_reset_cpu(struct sol1_cpu *sol1_cpu);


void debugmenu_main(struct sol1_cpu *sol1_cpu);

#endif