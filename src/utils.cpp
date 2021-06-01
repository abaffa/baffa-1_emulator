#include "utils.h"
#include <math.h>

#ifdef __linux__
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>


int getch(void)
{
	int ch = EOF;
	struct termios oldt, newt;
	int oldf;

	while(ch == EOF){

		tcgetattr(STDIN_FILENO, &oldt);
		newt = oldt;
		newt.c_lflag &= ~(ICANON | ECHO);
		tcsetattr(STDIN_FILENO, TCSANOW, &newt);
		oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
		fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

		ch = getchar();

		tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
		fcntl(STDIN_FILENO, F_SETFL, oldf);

	}

	return ch;
}

int kbhit(void)
{
	struct termios oldt, newt;
	int ch;
	int oldf;

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

	ch = getchar();

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);

	if (ch != EOF)
	{
		ungetc(ch, stdin);
		return 1;
	}

	return 0;
}

char* itoa(int value, char* result, int base) {
	// check that the base if valid
	if (base < 2 || base > 36) { *result = '\0'; return result; }

	char* ptr = result, *ptr1 = result, tmp_char;
	int tmp_value;

	do {
		tmp_value = value;
		value /= base;
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + (tmp_value - value * base)];
	} while (value);

	// Apply negative sign
	if (tmp_value < 0) *ptr++ = '-';
	*ptr-- = '\0';
	while (ptr1 < ptr) {
		tmp_char = *ptr;
		*ptr-- = *ptr1;
		*ptr1++ = tmp_char;
	}
	return result;
}
#else
#include <conio.h>
#endif

void print_word_bin(SOL1_DWORD n) {
	SOL1_BYTE h = MSB(n);
	SOL1_BYTE l = LSB(n);

	printf("%02x ", h);
	printf("%c%c%c%c%c%c%c%c", BYTE_TO_BINARY(h));
	printf(" ");
	printf("%02x ", l);
	printf("%c%c%c%c%c%c%c%c", BYTE_TO_BINARY(l));
}

void print_word_bin_nibbles(SOL1_DWORD n) {
	SOL1_BYTE bh = MSB(n);
	SOL1_BYTE bl = LSB(n);

	SOL1_BYTE bhnh = MSN(bh);
	SOL1_BYTE bhnl = LSN(bh);

	SOL1_BYTE blnh = MSN(bl);
	SOL1_BYTE blnl = LSN(bl);

	printf("%01x ", bhnh);
	printf("%c%c%c%c", NIBBLE_TO_BINARY(bhnh));
	printf(" ");

	printf("%01x ", bhnl);
	printf("%c%c%c%c", NIBBLE_TO_BINARY(bhnl));
	printf(" ");

	printf("%01x ", blnh);
	printf("%c%c%c%c", NIBBLE_TO_BINARY(blnh));
	printf(" ");

	printf("%01x ", blnl);
	printf("%c%c%c%c", NIBBLE_TO_BINARY(blnl));
}


void print_byte_bin(SOL1_BYTE b) {
	printf("%02x ", b);
	printf("%c%c%c%c%c%c%c%c", BYTE_TO_BINARY(b));
}

void print_nibble_bin(SOL1_BYTE b) {
	printf("%01x ", b);
	printf("%c%c%c%c", NIBBLE_TO_BINARY(b));
}


unsigned int toInt(char c)
{
	if (c >= '0' && c <= '9') return      c - '0';
	if (c >= 'A' && c <= 'F') return 10 + c - 'A';
	if (c >= 'a' && c <= 'f') return 10 + c - 'a';
	return -1;
}



void leftpad(char *str1, char *str2, int pad) {

	size_t p = pad - strlen(str1);
	int i = 0;
	for (i = 0; i < pad; i++) {
		if (i < p)
			str2[i] = '0';
		else
			str2[i] = str1[i - p];
	}

	str2[i] = '\0';

}

char* rightpad(const char *str1, int pad){

	char *str2 = (char*)malloc(pad + 1);

	size_t p = strlen(str1);
	int i = 0;
	for (i = 0; i < pad; i++) {
		if (i < p)
			str2[i] = str1[i];
		else
			str2[i] = ' ';
			
	}

	str2[i] = '\0';

	return str2;
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


SOL1_DWORD get_word_bit(SOL1_DWORD v, int bit) {
	if (bit == 0)
		return v & 0b0000000000000001;

	return (v & (0b0000000000000001 << bit)) >> bit;
}


SOL1_BYTE get_byte_bit(SOL1_BYTE v, int bit) {
	if (bit == 0)
		return v & 0b00000001;

	return (v & (0b00000001 << bit)) >> bit;
}

SOL1_BYTE check_byte_bit(SOL1_BYTE v, int bit) {
	if (bit == 0)
		return (v & 0b00000001) != 0x00;

	return (v & (0b00000001 << bit)) != 0x00;
}


SOL1_BYTE set_byte_bit(SOL1_BYTE v, int bit) {
	if (bit == 0)
		return v & 0b00000001;

	return (v & 0b00000001) << bit;
}


SOL1_DWORD set_word_bit(SOL1_DWORD v, int bit) {
	if (bit == 0)
		return v & 0b00000001;

	return (v & 0b00000001) << bit;
}




char* loadfile(char *filename, long *size) {

	printf("The filename to load is: %s", filename);

	FILE* f = fopen(filename, "rb");
	if (!f)
	{
		printf(" | Failed to open the file.\n");
		return NULL;
	}

	fseek(f, 0, SEEK_END);
	*size = ftell(f);
	fseek(f, 0, SEEK_SET);

	char* buf = (char*)malloc((*size) * sizeof(char));

	size_t res = fread(buf, *size, 1, f);
	if (res != 1)
	{
		printf(" | Failed to read from file.\n");
		return NULL;
	}

	printf(" | OK.\n");
	return buf;
}

void save_to_log(FILE *fa, char *str)
{

	//FILE *fa = fopen("File1.txt", "a");
	if (fa == NULL)
	{
		printf("can not open target file\n");
		exit(1);
	}

	fprintf(fa, "%s", str);
	fflush(fa);
	//fputc(str, fa);

//fclose(fa);

}



void reg8bit_print(FILE *fa, char *dir, char *reg, SOL1_BYTE value) {
	char line[255];
	sprintf(line, "         \t\t\t\t%s REG\t %s \t= %02x\n", dir, reg, value);
	save_to_log(fa, line);
}



char* strlower(char* s)
{
	char* tmp = s;

	for (; *tmp; ++tmp) {
		*tmp = tolower((unsigned char)*tmp);
	}

	return s;
}