#ifndef SOL1ALU_H
#define SOL1ALU_H

#include "sol1_alu_4bit.h"
#include "sol1_alu_8bit.h"
#include "config.h"


SOL1_BYTE ALU_EXEC(sol1_alu_8bit *alu, SOL1_BYTE x_bus, SOL1_BYTE y_bus,
	SOL1_BYTE alu_op,
	SOL1_BYTE alu_mode,
	SOL1_BYTE alu_cf_in_src,
	SOL1_BYTE alu_cf_in_inv,
	SOL1_BYTE alu_cf_out_inv,
	SOL1_BYTE u_cf, SOL1_BYTE msw_cf, SOL1_BYTE shift_src, SOL1_BYTE zbus_out_src);

#endif;