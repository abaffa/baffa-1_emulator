#ifndef DEBUGMENU_ROMS_H
#define DEBUGMENU_ROMS_H

#include "sol1_cpu.h"

void debugmenu_roms_menu(HW_TTY& hw_tty);


void debugmenu_roms(SOL1_CPU& sol1_cpu, HW_TTY& hw_tty);
void debugmenu_roms_edit_breakpoint(SOL1_ROM& sol1_rom, HW_TTY& hw_tty);
#endif