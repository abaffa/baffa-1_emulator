#ifndef SOL1CPU_H
#define SOL1CPU_H

#include "config.h"
#include "sol1_rom.h"
#include "sol1_memory.h"
#include "sol1_microcode.h"
#include "sol1_registers.h"
#include <stddef.h>

struct sol1_cpu
{
    struct sol1_memory memory;
	struct sol1_registers registers;

	struct sol1_rom rom;

	struct sol1_microcode microcode;

    char last_op_desc[512];

	SOL1_MWORD BKPT; //breakpoint (FFFF = disable)
};

void sol1_cpu_init(struct sol1_cpu* sol1_cpu);
//void sol1_cpu_load(struct z80* z80, const char* buf, size_t size, unsigned short A);
//void sol1_cpu_exec(struct z80* z80);
void sol1_cpu_reset(struct sol1_cpu *sol1_cpu);
void sol1_cpu_display_registers(struct sol1_cpu *sol1_cpu);
void sol1_cpu_display_registers_lite(struct sol1_cpu *sol1_cpu);
void sol1_cpu_memory_display(struct sol1_cpu *sol1_cpu);

/*
unsigned char RdZ80(struct z80* z80, unsigned short Address);
void WrZ80(struct z80* z80, unsigned short Address, unsigned char V);
void IntZ80(struct z80* z80, unsigned short Vector);
void JumpZ80(struct z80* z80, unsigned short PC);
*/

void mswh_flags_desc(struct sol1_cpu* sol1_cpu);
void mswl_status_desc(struct sol1_cpu* sol1_cpu);

void load_microcode_from_rom(struct sol1_cpu *sol1_cpu,
	sol1_alu_8bit *alu);




SOL1_BYTE refresh_MSWh_ZF(sol1_cpu *sol1_cpu, sol1_alu_8bit *alu, SOL1_BYTE z_bus, SOL1_BYTE zf_in_src);
SOL1_BYTE refresh_MSWh_CF(sol1_cpu *sol1_cpu, sol1_alu_8bit *alu, SOL1_BYTE z_bus, SOL1_BYTE cf_in_src);
SOL1_BYTE refresh_MSWh_SF(sol1_cpu *sol1_cpu, sol1_alu_8bit *alu, SOL1_BYTE z_bus, SOL1_BYTE sf_in_src);
SOL1_BYTE refresh_MSWh_OF(sol1_cpu *sol1_cpu, sol1_alu_8bit *alu, SOL1_BYTE z_bus, SOL1_BYTE u_sf, SOL1_BYTE of_in_src);


#endif