#include "config.h"

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


#define WORD(l, h) (((h)<<8) | (l))
#define LSB(w) ((w) & 0xff)
#define MSB(w) (((w) >> 8) & 0xff)
#define SET_LSB(Rg, l) Rg = ((Rg & 0xFF00) | ((l) & 0xff))
#define SET_MSB(Rg, h) Rg = ((h)<<8) | (Rg & 0x00FF)
#define SET_WORD(Rg, l, h) Rg = WORD(l, h)

void print_word_bin(SOL1_DWORD n) {
	SOL1_BYTE h = MSB(n);
	SOL1_BYTE l = LSB(n);

	printf("%02x ", h);
	printf("%c%c%c%c%c%c%c%c", BYTE_TO_BINARY(h));
	printf(" ");
	printf("%02x ", l);
	printf("%c%c%c%c%c%c%c%c", BYTE_TO_BINARY(l));
	printf("\n");
}

void print_byte_bin(SOL1_BYTE b) {
	printf("%02x ", b);
	printf("%c%c%c%c%c%c%c%c", BYTE_TO_BINARY(b));
	printf("\n");
}

unsigned int toInt(char c)
{
	if (c >= '0' && c <= '9') return      c - '0';
	if (c >= 'A' && c <= 'F') return 10 + c - 'A';
	if (c >= 'a' && c <= 'f') return 10 + c - 'a';
	return -1;
}



void leftpad(char *str1, char *str2, int pad) {

	int p = pad - strlen(str1);
	int i = 0;
	for (i = 0; i < pad; i++) {
		if (i < p)
			str2[i] = '0';
		else
			str2[i] = str1[i - p];
	}

	str2[i] = '\0';

}

unsigned int convert_hexstr_to_value(char *value) {
	size_t numdigits = strlen(value) / 2;
	size_t i;
	unsigned int address = 0;
	for (i = 0; i != numdigits; ++i)
	{
		address = address << 8;

		unsigned char output = 16 * toInt(value[2 * i]) + toInt(value[2 * i + 1]);
		address += output;
	}
	return address;
}

char* gets(int max_value) {

	char *input = (char*)malloc(sizeof(char) * 257);

	int i = 0;
	for (i = 0; i < 256 && i < max_value; ) {
		char cur_input = getch();
		if (cur_input == (char)8) {
			if (i > 0) {
				printf("%c", cur_input);
				printf(" ");
				printf("%c", cur_input);
				i--;
			}
		}
		else if (cur_input != '\n' && cur_input != '\r') {
			cur_input = toupper(cur_input);
			printf("%c", cur_input);
			input[i] = cur_input;
			i++;
		}
		else
			break;
	}
	input[i] = '\0';
	return input;
}