#ifndef DEBUGMENU_ROMS_H
#define DEBUGMENU_ROMS_H

#include "sol1_cpu.h"

void debugmenu_roms_menu();


void debugmenu_roms(struct sol1_cpu *sol1_cpu);
void debugmenu_roms_edit_breakpoint(struct sol1_rom *sol1_rom);
#endif