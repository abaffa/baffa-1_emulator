#ifndef DEBUGMENU_MAIN_H
#define DEBUGMENU_MAIN_H

#include "sol1_cpu.h"

void debugmenu_main_menu();
void debugmenu_main_disassemble_mem();
void debugmenu_main_edit_mem(SOL1_MEMORY& memory);
void debugmenu_main_fill_mem(SOL1_MEMORY& memory);
void debugmenu_main_load_mem(SOL1_CPU& sol1_cpu);
void debugmenu_main_display_registers(SOL1_CPU& sol1_cpu);
void debugmenu_main_edit_register(SOL1_CPU& sol1_cpu);
void debugmenu_main_edit_breakpoint(SOL1_CPU& sol1_cpu);
void debugmenu_main_edit_programcounter(SOL1_CPU& sol1_cpu);
void debugmenu_main_reset_cpu(SOL1_CPU& sol1_cpu);


int debugmenu_main(SOL1_CPU& sol1_cpu);

#endif