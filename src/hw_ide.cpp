#include "hw_ide.h"
#include <stdlib.h>
#include <assert.h>

void hw_ide_init(struct hw_ide* hw_ide) {

	int address = 0;

	hw_ide->memory = (SOL1_BYTE*)malloc(SOL1_IDE_MEMORY_SIZE * sizeof(SOL1_BYTE));

	for (address = 0; address < SOL1_IDE_MEMORY_SIZE; address++) {
		hw_ide->memory[address] = 0x00;
	}
}

void hw_ide_write(struct hw_ide* hw_ide, SOL1_BYTE b) {

	
}

void hw_ide_read(struct hw_ide* hw_ide) {
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