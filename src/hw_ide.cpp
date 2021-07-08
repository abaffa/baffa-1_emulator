//
// hw_ide.cpp
//
////// BEGIN LICENSE NOTICE//////
//
//Sol-1 HomebrewCPU Minicomputer System Emulator
//
//Copyright(C) 2021 Augusto Baffa, (sol-1.baffasoft.com.br)
//
//This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110 - 1301, USA.
//
////// END LICENSE NOTICE//////
//
#include "hw_ide.h"
#include <stdlib.h>
#include <assert.h>

void hw_ide_init(struct hw_ide* hw_ide) {

	int address = 0;

	hw_ide->memory = (SOL1_BYTE*)malloc(SOL1_IDE_MEMORY_SIZE * sizeof(SOL1_BYTE));

	for (address = 0; address < SOL1_IDE_MEMORY_SIZE; address++) {
		hw_ide->memory[address] = 0x00;
	}

	hw_ide_reset(hw_ide);
}

void hw_ide_reset(struct hw_ide* hw_ide) {
	hw_ide->gambi_ide_total = 0;
	hw_ide->gambi_ide_read = 0;
}


void hw_ide_write(struct hw_ide* hw_ide) {
	if (hw_ide->data[7] == 0b00001000) {

		hw_ide->gambi_ide_total = hw_ide->data[2];

		unsigned long sec_address_lba = hw_ide->data[3];
		sec_address_lba = sec_address_lba | ((unsigned long)hw_ide->data[4]) << 8;
		sec_address_lba = sec_address_lba | ((unsigned long)hw_ide->data[5]) << 16;
		sec_address_lba = sec_address_lba | ((unsigned long)(hw_ide->data[6] & 0b00001111)) << 24;

		unsigned long sec_address_byte = sec_address_lba * 512;

		if (sec_address_byte < SOL1_IDE_MEMORY_SIZE) {
			hw_ide->memory[sec_address_byte + hw_ide->gambi_ide_read] = hw_ide->data[0];

			hw_ide->gambi_ide_read++;

			if (hw_ide->gambi_ide_read > hw_ide->gambi_ide_total * 512)
			{
				hw_ide->data[7] = 0x00;
				hw_ide_reset(hw_ide);

				hw_ide_save_disk(hw_ide->memory);
			}
		}
		else {
			hw_ide->data[7] = 0x34;
			hw_ide_reset(hw_ide);
		}
	}

}

void hw_ide_read(struct hw_ide* hw_ide) {

	if (hw_ide->data[7] == 0b00001000) {

		hw_ide->gambi_ide_total = hw_ide->data[2];

		unsigned long sec_address_lba = hw_ide->data[3];
		sec_address_lba = sec_address_lba | ((unsigned long)hw_ide->data[4]) << 8;
		sec_address_lba = sec_address_lba | ((unsigned long)hw_ide->data[5]) << 16;
		sec_address_lba = sec_address_lba | ((unsigned long)(hw_ide->data[6] & 0b00001111)) << 24;

		unsigned long sec_address_byte = sec_address_lba * 512;

		if (sec_address_byte < SOL1_IDE_MEMORY_SIZE) {
			hw_ide->data[0] = hw_ide->memory[sec_address_byte + hw_ide->gambi_ide_read];

			hw_ide->gambi_ide_read++;

			if (hw_ide->gambi_ide_read > hw_ide->gambi_ide_total * 512)
			{
				hw_ide->data[7] = 0x00;
				hw_ide_reset(hw_ide);
			}
		}
		else {
			hw_ide->data[7] = 0x24;
			hw_ide_reset(hw_ide);
		}
	}

}



void hw_ide_save_disk(SOL1_BYTE *data) {
	FILE *file = fopen("data.dsk", "wb");
	if (file != NULL) {
		fwrite(data, sizeof(SOL1_BYTE), SOL1_IDE_MEMORY_SIZE, file);
		fclose(file);
	}
}

void hw_ide_load_disk(SOL1_BYTE *data) {
	FILE *file = fopen("data.dsk", "rb");
	if (file != NULL) {
		fread(data, sizeof(SOL1_BYTE), SOL1_IDE_MEMORY_SIZE, file);
		fclose(file);
	}
}




void hw_ide_print(struct hw_ide* hw_ide, char *dir, int changed, char *print) {

	int i = 0;
	sprintf(print, ">>> IDE [%s]: ", dir);
	for (i = 0; i < 7; i++) {

		if (changed == i)
			sprintf(print + strlen(print), "[%02x(%d)", hw_ide->data[i], hw_ide->data[i]);
		else if (changed == i - 1)
			sprintf(print + strlen(print), "]%02x(%d)", hw_ide->data[i], hw_ide->data[i]);
		else
			sprintf(print + strlen(print), " %02x(%d)", hw_ide->data[i], hw_ide->data[i]);
	}

	if (changed == i)
		sprintf(print + strlen(print), "[%02x(%d)", hw_ide->data[i], hw_ide->data[i]);
	else if (changed == i - 1)
		sprintf(print + strlen(print), "]%02x(%d)", hw_ide->data[i], hw_ide->data[i]);
	else
		sprintf(print + strlen(print), " %02x(%d)", hw_ide->data[i], hw_ide->data[i]);

	if (changed == i)
		sprintf(print + strlen(print), "]\n");
	else
		sprintf(print + strlen(print), "\n");

}