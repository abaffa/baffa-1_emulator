#ifndef SOL1CPU_H
#define SOL1CPU_H

#include "config.h"
#include "sol1_memory.h"
#include "sol1_registers.h"
#include <stddef.h>

//                               /* LoopZ80() may return:      */
//#define INT_RST00   0x00C7     /* RST 00h                    */
//#define INT_RST08   0x00CF     /* RST 08h                    */
//#define INT_RST10   0x00D7     /* RST 10h                    */
//#define INT_RST18   0x00DF     /* RST 18h                    */
//#define INT_RST20   0x00E7     /* RST 20h                    */
//#define INT_RST28   0x00EF     /* RST 28h                    */
//#define INT_RST30   0x00F7     /* RST 30h                    */
//#define INT_RST38   0x00FF     /* RST 38h                    */
//#define INT_IRQ     INT_RST38  /* Default IRQ opcode is FFh  */
//#define INT_NMI     0xFFFD     /* Non-maskable interrupt     */
//#define INT_NONE    0xFFFF     /* No interrupt required      */
//#define INT_QUIT    0xFFFE     /* Exit the emulation         */
//
//                               /* Bits in Z80 F register:    */
//#define S_FLAG      0x80       /* 1: Result negative         */
//#define Z_FLAG      0x40       /* 1: Result is zero          */
//#define X2_FLAG     0x20       /* UNUSED BITS    */
//#define H_FLAG      0x10       /* 1: Halfcarry/Halfborrow    */
//#define X1_FLAG     0x08       /* UNUSED BITS    */
//#define P_FLAG      0x04       /* 1: Result is even          */
//#define V_FLAG      0x04       /* 1: Overflow occured        */
//#define N_FLAG      0x02       /* 1: Subtraction occured     */
//#define C_FLAG      0x01       /* 1: Carry/Borrow occured    */
//
//#define IFF_1       0x01       /* IFF1 flip-flop             */
//#define IFF_IM1     0x02       /* 1: IM1 mode                */
//#define IFF_IM2     0x04       /* 1: IM2 mode                */
//#define IFF_2       0x08       /* IFF2 flip-flop             */
//#define IFF_EI      0x20       /* 1: EI pending              */
//#define IFF_HALT    0x80       /* 1: CPU HALTed              */


struct sol1_cpu
{
    struct sol1_memory memory;
    struct sol1_registers registers;

    char last_op_desc[512];

	SOL1_MWORD BKPT; //breakpoint (FFFF = disable)
};

void sol1_cpu_init(struct sol1_cpu* sol1_cpu);
//void sol1_cpu_load(struct z80* z80, const char* buf, size_t size, unsigned short A);
//void sol1_cpu_exec(struct z80* z80);
void sol1_cpu_reset(struct sol1_cpu *sol1_cpu);
void sol1_cpu_display_registers(struct sol1_cpu *sol1_cpu);

/*
unsigned char RdZ80(struct z80* z80, unsigned short Address);
void WrZ80(struct z80* z80, unsigned short Address, unsigned char V);
void IntZ80(struct z80* z80, unsigned short Vector);
void JumpZ80(struct z80* z80, unsigned short PC);
*/
#endif