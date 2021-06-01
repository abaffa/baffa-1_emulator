#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define EMULATOR_WINDOW_TITLE "SOL-1 Emulator"

#define SOL1_BIOS_MEMORY_SIZE (0xFFFF + 1)
#define SOL1_PAGING_MEMORY_SIZE (0x7FFF + 1)
#define SOL1_MAIN_MEMORY_SIZE	(0x3FFFFF + 1)


#define SOL1_IDE_MEMORY_SIZE	(0x3FFFFF + 1)

#define SOL1_ROM_SIZE		(0xFFFF + 1)
#define SOL1_TOTAL_KEYS		41


// ROMS
#define SOL1_ROM_NBR_ROMS 15
#define SOL1_ROM_TOTAL_CONTROL_BITS NBR_ROMS * 8
#define SOL1_ROM_CYCLES_PER_INSTR 64
#define SOL1_ROM_NBR_INSTRUCTIONS 256
#define SOL1_ROM_DESC	0x410000

#define SOL1_BYTE				unsigned char // 8bit
#define SOL1_MWORD				unsigned int //Memory Word (int 32bit, short 16bit, char 8bit)
#define SOL1_DWORD				unsigned int //Data Word




#define INI_SERVER 0
#define INI_WEB_SERVER 0

//#define INI_DEBUG_OPCODE 0

#define INI_DEBUG_MICROCODE 0
#define INI_DEBUG_UADDRESSER 0
#define INI_DEBUG_UADDER 0
#define INI_DEBUG_UFLAGS 0


#define INI_DEBUG_LITE 0

#define INI_DEBUG_BUSES 0	
#define INI_DEBUG_ALU 0

#define INI_DEBUG_RDREG 0
#define INI_DEBUG_WRREG 0
#define INI_DEBUG_REGISTERS 0

#define INI_DEBUG_RDMEM 0
#define INI_DEBUG_WRMEM 0
#define INI_DEBUG_MEMORY 0

#define INI_DEBUG_UART 0
#define INI_DEBUG_IDE 0
#define INI_DEBUG_RTC 0
#define INI_DEBUG_TIMER 0

#define INI_DEBUG_LOG_OPCODE 0

/// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//boot sequence: bios, boot, kernel, shell
/// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#endif