#ifndef UTILS_H
#define UTILS_H

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

#define INV_BYTE_TO_BINARY(byte)  \
  (byte & 0x01 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x80 ? '1' : '0')

#define NIBBLE_TO_BINARY(byte)  \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

#define LSN(w) ((w) & 0x0f)
#define MSN(w) (((w) >> 4) & 0x0f)

#define WORD(l, h) (((h)<<8) | (l))
#define LSB(w) ((w) & 0xff)
#define MSB(w) (((w) >> 8) & 0xff)
#define SET_LSB(Rg, l) Rg = ((Rg & 0xFF00) | ((l) & 0xff))
#define SET_MSB(Rg, h) Rg = ((h)<<8) | (Rg & 0x00FF)
#define SET_WORD(Rg, l, h) Rg = WORD(l, h)

void print_word_bin(SOL1_DWORD n);
void print_word_bin_nibbles(SOL1_DWORD n);

void print_byte_bin(SOL1_BYTE b);

void print_nibble_bin(SOL1_BYTE b);

unsigned int toInt(char c);



void leftpad(char *str1, char *str2, int pad);
char* rightpad(const char *str1, int pad);

unsigned int convert_hexstr_to_value(char *value);

char* gets(int max_value);

SOL1_DWORD set_word_bit(SOL1_DWORD v, int bit);

SOL1_DWORD get_word_bit(SOL1_DWORD v, int bit);
SOL1_BYTE get_byte_bit(SOL1_BYTE v, int bit);
SOL1_BYTE check_byte_bit(SOL1_BYTE v, int bit);
SOL1_BYTE set_byte_bit(SOL1_BYTE v, int bit);

char* loadfile(char *filename, long *size);

void save_to_log(FILE *fa, char *str);

char* strlower(char* s);
#endif