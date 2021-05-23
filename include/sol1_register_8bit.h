#ifndef SOL1REGISTER_H
#define SOL1REGISTER_H

#include "config.h"

typedef struct sol1_register_8bit SOL1_REGISTER_8BIT;


SOL1_REGISTER_8BIT *sol1_register_8bit_create();

void sol1_register_8bit_destroy(SOL1_REGISTER_8BIT *reg);

SOL1_BYTE sol1_register_8bit_value(SOL1_REGISTER_8BIT *reg);

void sol1_register_8bit_reset(SOL1_REGISTER_8BIT *reg);

void sol1_register_8bit_set(SOL1_REGISTER_8BIT *reg, SOL1_BYTE v);


#endif;