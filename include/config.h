#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define EMULATOR_WINDOW_TITLE "SOL-1 Emulator"

#define Z80_PROGRAM_LOAD_ADDRESS 0X000

//#define ZX80_HEIGHT 232 // 26 x 8

#define Z80_TOTAL_DATA_REGISTERS 16
#define Z80_TOTAL_STACK_DEPTH 16

#define Z80_CHARACTER_SET_LOAD_ADDRESS 0x00
#define Z80_DEFAULT_SPRITE_HEIGHT 5

#define SOL1_WINDOW_MULTIPLIER 2

#define SOL1_WIDTH			256 //32 x 8
#define SOL1_HEIGHT			200 // 25 x 8
#define SOL1_MEMORY_SIZE	(0x3FFFFF + 1)
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




#define SERVER 0


#define DEBUG_MICROCODE 0
#define DEBUG_UADDRESSER 0
#define DEBUG_UADDER 0
#define DEBUG_UFLAGS 0
#define DEBUG_REGISTERS 0
#define DEBUG_BUSES 0	
#define DEBUG_ALU 0
#define DEBUG_RDMEM 0
#define DEBUG_WRMEM 0
#define DEBUG_MEMORY 0
#define DEBUG_LITE 0
/// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//boot sequence: bios, boot, kernel, shell
/// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#endif