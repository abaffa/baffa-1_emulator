#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include <math.h>

#define B_BYTE unsigned char

#define NBR_ROMS 15
#define TOTAL_CONTROL_BITS NBR_ROMS * 8
#define CYCLES_PER_INSTR 64
#define NBR_INSTRUCTIONS 256

int state = 0;
unsigned short mem_offset = 0x0000;
int mem_follow = 0;

B_BYTE memory; //[z80_MEMORY_SIZE];

const char* control_list[] = { "next_0", "next_1", "offset_0", "offset_1", "offset_2", "offset_3", "offset_4", "offset_5", "offset_6", "cond_inv", "cond_flags_src", "cond_sel_0",
		 "cond_sel_1", "cond_sel_2", "cond_sel_3", "ESCAPE", "uzf_in_src_0", "uzf_in_src_1", "ucf_in_src_0", "ucf_in_src_1", "usf_in_src", "uof_in_src", "IR_wrt", "status_wrt",
		  "shift_src_0", "shift_src_1", "shift_src_2", "zbus_out_src_0", "zbus_out_src_1", "alu_a_src_0", "alu_a_src_1", "alu_a_src_2", "alu_a_src_3", "alu_a_src_4", "alu_a_src_5",
		   "alu_op_0", "alu_op_1", "alu_op_2", "alu_op_3", "alu_mode", "alu_cf_in_src_0", "alu_cf_in_src_1", "alu_cf_in_inv", "zf_in_src_0", "zf_in_src_1", "alu_cf_out_inv",
			"cf_in_src_0", "cf_in_src_1", "cf_in_src_2", "sf_in_src_0", "sf_in_src_1", "of_in_src_0", "of_in_src_1", "of_in_src_2", "rd", "wr", "alu_b_src_0", "alu_b_src_1",
			 "alu_b_src_2", "display_reg_load", "dl_wrt", "dh_wrt", "cl_wrt", "ch_wrt", "bl_wrt", "bh_wrt", "al_wrt", "ah_wrt", "mdr_in_src", "mdr_out_src", "mdr_out_en",
			  "mdrl_wrt", "mdrh_wrt", "tdrl_wrt", "tdrh_wrt", "dil_wrt", "dih_wrt", "sil_wrt", "sih_wrt", "marl_wrt", "marh_wrt", "bpl_wrt", "bph_wrt", "pcl_wrt", "pch_wrt",
			  "spl_wrt", "sph_wrt", "-", "-", "int_vector_wrt", "mask_flags_wrt", "mar_in_src", "int_ack", "clear_all_ints", "ptb_wrt", "pagtbl_ram_we", "mdr_to_pagtbl_en",
			  "force_user_ptb", "-", "-", "-", "-", "gl_wrt", "gh_wrt", "imm_0", "imm_1", "imm_2", "imm_3", "imm_4", "imm_5", "imm_6", "imm_7", "-", "-", "-", "-", "-", "-", "-", "-" };


void cls(HANDLE hConsole)
{
	COORD coordScreen = { 0, 0 };    // home for the cursor
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD dwConSize;

	// Get the number of character cells in the current buffer.
	if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
	{
		return;
	}

	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

	// Fill the entire screen with blanks.
	if (!FillConsoleOutputCharacter(hConsole,        // Handle to console screen buffer
		(TCHAR)' ',      // Character to write to the buffer
		dwConSize,       // Number of cells to write
		coordScreen,     // Coordinates of first cell
		&cCharsWritten)) // Receive number of characters written
	{
		return;
	}

	// Get the current text attribute.
	if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
	{
		return;
	}

	// Set the buffer's attributes accordingly.
	if (!FillConsoleOutputAttribute(hConsole,         // Handle to console screen buffer
		csbi.wAttributes, // Character attributes to use
		dwConSize,        // Number of cells to set attribute
		coordScreen,      // Coordinates of first cell
		&cCharsWritten))  // Receive number of characters written
	{
		return;
	}

	// Put the cursor at its home coordinates.
	SetConsoleCursorPosition(hConsole, coordScreen);
}
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

#define LSB(w) ((w) & 0xff)
#define MSB(w) (((w) >> 8) & 0xff)

#define INV_BYTE_TO_BINARY(byte)  \
  (byte & 0x01 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x80 ? '1' : '0')
/*
void cpu_print(struct z80 *z80){


			printf("\t "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(MSB(z80->registers.AF)));

			//if(state == 0){
			printf("  AF  |  BC  |  DE  |  HL  || S | Z | H | P | V | N | C |\n");
			printf(" %04x |", z80->registers.AF);
			printf(" %04x |", z80->registers.BC);
			printf(" %04x |", z80->registers.DE);
			printf(" %04x ||", z80->registers.HL);
			printf(" %01x |", (z80->registers.AF & S_FLAG) >> 7);
			printf(" %01x |", (z80->registers.AF & Z_FLAG) >> 6);
			printf(" %01x |", (z80->registers.AF & H_FLAG) >> 4);
			printf(" %01x |", (z80->registers.AF & P_FLAG) >> 2);
			printf(" %01x |", (z80->registers.AF & V_FLAG) >> 2);
			printf(" %01x |", (z80->registers.AF & N_FLAG) >> 1);
			printf(" %01x |", (z80->registers.AF & C_FLAG));
			printf("FLAGS "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(LSB(z80->registers.AF)));
			printf("\n\n");

			printf("  AF' |  BC' |  DE' |  HL' ||  PC  |  SP  ||  IX  |  IY  || I  | R  |\n");
			printf(" %04x |", z80->registers.AFl);
			printf(" %04x |", z80->registers.BCl);
			printf(" %04x |", z80->registers.DEl);
			printf(" %04x ||", z80->registers.HLl);
			printf(" %04x |", z80->registers.PC);
			printf(" %04x ||", z80->registers.SP);
			printf(" %04x |", z80->registers.IX);
			printf(" %04x ||", z80->registers.IY);
			printf(" %02x |", z80->registers.I);
			printf(" %02x |", z80->registers.R);
			printf("\n\n");
			printf("IFF "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(z80->registers.IFF));
			printf("\n\n");


				printf("%x", mem_follow);
				if(mem_follow)
					mem_offset  = (z80->registers.PC / 0x10) * 0x10;

				int i = 0;
				printf("offset: %04x\n", mem_offset);
				printf("        ");
				for(i = 0; i < 16; i++)
					printf("%02x ", i);
				printf("\n\n %04x ", mem_offset);

				for(i = 0 + mem_offset; i < 256 + mem_offset; i++){
					if(i % 16 == 0)
						if(z80->registers.PC == i)
							printf(" #");
						else
							printf("  ");
					if(z80->registers.PC == i || z80->registers.PC-1 == i)
						printf("%02x#", RdZ80(z80, i));
					else
						printf("%02x ", RdZ80(z80, i));

					if((i+1) % 16 == 0 && i < 255+ mem_offset)
						printf("\n %04x ", i+1);
				}
				printf("\n\n");
			//}
}
*/

unsigned int toInt(char c)
{
	if (c >= '0' && c <= '9') return      c - '0';
	if (c >= 'A' && c <= 'F') return 10 + c - 'A';
	if (c >= 'a' && c <= 'f') return 10 + c - 'a';
	return -1;
}

int run = 0;

int load_rom(const char* filename, B_BYTE* rom) {

	printf("The filename to load is: %s\n", filename);

	FILE* f = fopen(filename, "rb");
	if (!f)
	{
		printf("Failed to open the file");
		return 0;
	}

	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	fseek(f, 0, SEEK_SET);

	char* buf = (char*)malloc(size * sizeof(char));

	int res = fread(buf, size, 1, f);
	if (res != 1)
	{
		printf("Failed to read from file");
		return 0;
	}

	int i;
	for (i = 0; i < size; i++)
		rom[i] = buf[i];

	return 1;
}


int main(int argc, char** argv)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	cls(hConsole);





	//B_BYTE** roms = (B_BYTE**)malloc(sizeof(B_BYTE) * NBR_ROMS * (NBR_INSTRUCTIONS * CYCLES_PER_INSTR));
	B_BYTE* rom_desc = (B_BYTE*)malloc(sizeof(B_BYTE) * 0x410000);
	B_BYTE roms[NBR_ROMS][(NBR_INSTRUCTIONS * CYCLES_PER_INSTR)];

	load_rom("rom", rom_desc);

	int _roms = 0;
	for (_roms = 0; _roms < NBR_ROMS; _roms++) {
		char filename[20];
		sprintf(filename, "rom%d", _roms);
		load_rom(filename, roms[_roms]);
	}


	cls(hConsole);
	SetConsoleCursorPosition(hConsole, { 0, 0 });
	unsigned char output = 0;
	int cycle = 0;

	while (1) {
		char *input = (char*)malloc(sizeof(char) * 257);

		printf(">> ");
		scanf("%s", input);

		if (input[0] != 'n' && input[0] != 'N' &&
			input[0] != 'p' && input[0] != 'P') {
			size_t numdigits = strlen(input) / 2;
			size_t i;
			for (i = 0; i != numdigits && i < 1; ++i)
			{
				//unsigned char output
				output = 16 * toInt(input[2 * i]) + toInt(input[2 * i + 1]);
				cycle = 0;
			}

		}
		else if (input[0] == 'n' || input[0] == 'N') {
			if (cycle < CYCLES_PER_INSTR - 1)
				cycle++;
		}
		else if (input[0] == 'P' || input[0] == 'P') {
			if (cycle > 0)
				cycle--;
		}
		free(input);



		///////////////////////////////////////
		cls(hConsole);
		SetConsoleCursorPosition(hConsole, { 0, 0 });
		int j = 0;

		printf("Inst.: %02x %s\n", output, &rom_desc[0x400000 + (output * 256)]);
		printf("Cycle: %02x\n", cycle);

		printf("\n");
		for (j = 0; j < NBR_ROMS; j++)
			printf(" Rom %02d  ", j);
		printf("\n");

		for (j = 0; j < NBR_ROMS; j++) {
			int p = output * CYCLES_PER_INSTR + cycle;
			printf(""BYTE_TO_BINARY_PATTERN" ", INV_BYTE_TO_BINARY(roms[j][p]));
		}
		printf("\n\n");

		int k, l;
		for (j = 0; j < 24; j++)
		{
			for (k = 0; k < 5; k++) {


				if (k > 0)
					for (l = 0; l < 4 - (strlen(control_list[j + 24 * (k - 1)]) + 2) / 8; l++)
						printf("\t");


				int c_rom = (j + 24 * k) / 8;
				int c_p = output * CYCLES_PER_INSTR + cycle;
				char c_bit = pow(2, (j + 24 * k) % 8);
				unsigned char c_byte = roms[c_rom][c_p];

				if ((c_byte & c_bit) >> ((j + 24 * k) % 8) == 1)
					printf("*");
				else
					printf(" ");

				printf("%s", control_list[j + 24 * k]);

				if ((c_byte & c_bit) >> ((j + 24 * k) % 8) == 1)
					printf("*");
				else
					printf(" ");

			}
			printf("\n");
		}


	}

}