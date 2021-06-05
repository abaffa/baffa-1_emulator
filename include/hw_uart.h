#ifndef HWUART_H
#define HWUART_H

#include "queue_ll.h"

struct hw_uart {

	SOL1_BYTE status;
	
	Queue* uart_in;
	Queue* uart_out;
	SOL1_BYTE data[6];

	void *sol1_cpu;
};

void hw_uart_init(struct hw_uart* hw_uart, void *sol1_cpu);

int hw_uart_read(struct hw_uart* hw_uart);
int hw_uart_write(struct hw_uart* hw_uart);

SOL1_BYTE hw_uart_get_lsr(struct hw_uart* hw_uart);


void hw_uart_send(struct hw_uart* hw_uart, SOL1_BYTE b);
void hw_uart_receive(struct hw_uart* hw_uart, SOL1_BYTE b);

void hw_uart_print(struct hw_uart* hw_uart, char *dir, int changed, char *print);
#endif
