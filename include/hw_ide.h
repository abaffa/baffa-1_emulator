#ifndef HWIDE_H
#define HWIDE_H

#include <stdio.h>
#include "config.h"


struct hw_ide {

	int gambi_ide_total;
	int gambi_ide_read;

	SOL1_BYTE data[8];

	SOL1_BYTE* memory; 
};

void hw_ide_init(struct hw_ide* hw_ide);
void hw_ide_reset(struct hw_ide* hw_ide);

void hw_ide_read(struct hw_ide* hw_ide);
void hw_ide_write(struct hw_ide* hw_ide);

void hw_ide_print(struct hw_ide* hw_ide, char *dir, int changed, char *print);


void hw_ide_save_disk(SOL1_BYTE *data);
void hw_ide_load_disk(SOL1_BYTE *data);
#endif
