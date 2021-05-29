#ifndef DEBUGMENU_ROMS_H
#define DEBUGMENU_ROMS_H

#include "sol1_cpu.h"

void debugmenu_roms_menu();


void debugmenu_roms(SOL1_CPU& sol1_cpu);
void debugmenu_roms_edit_breakpoint(SOL1_ROM& sol1_rom);
#endif