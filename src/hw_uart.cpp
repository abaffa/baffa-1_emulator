#include <stdlib.h>
#include "sol1_cpu.h"
#include "utils.h"
#include "hw_uart.h"

struct uart_data {
	SOL1_BYTE data;
};


void hw_uart_init(struct hw_uart* hw_uart) {

	hw_uart->uart_in = queue_create();
	hw_uart->uart_out = queue_create();
	hw_uart->data[5] = 0xFF;

	hw_uart->status = 0x00;
}

int hw_uart_write(struct hw_uart* hw_uart) {

	if (!queue_empty(hw_uart->uart_in)) {
		struct uart_data *data = (struct uart_data*)queue_remove(hw_uart->uart_in);
		hw_uart->data[0] = data->data;
		printf("%c", hw_uart->data[0]);
		return 1;
	}
	return 0;
}

int hw_uart_read(struct hw_uart* hw_uart) {
	if (!queue_empty(hw_uart->uart_out)) {
		struct uart_data *data = (struct uart_data*)queue_remove(hw_uart->uart_out);
		hw_uart->data[0] = data->data;
		return 1;
	}

	return 0;
}

SOL1_BYTE hw_uart_get_lsr(struct hw_uart* hw_uart) {

	if (!queue_empty(hw_uart->uart_out) || !queue_empty(hw_uart->uart_in)) {
		hw_uart->data[5] = hw_uart->data[5] == 0x01 ? 0x20 : 0x01;
	}
	else {
		hw_uart->data[5] = queue_empty(hw_uart->uart_in) ? 0x20 : 0xFF;

	}


	return hw_uart->data[5];
}


void hw_uart_receive(struct hw_uart* hw_uart, SOL1_BYTE b) {

	struct uart_data *data = (struct uart_data*)malloc(sizeof(struct uart_data));
	data->data = b;
	queue_insert(hw_uart->uart_out, data);
}



void hw_uart_send(struct hw_uart* hw_uart, SOL1_BYTE b) {

	struct uart_data *data = (struct uart_data*)malloc(sizeof(struct uart_data));
	data->data = b;
	queue_insert(hw_uart->uart_in, data);
}


void hw_uart_print(struct hw_uart* hw_uart, char *dir, int changed, char *print) {

	int i = 0;
	sprintf(print, ">>> UART [%s]:", dir);
	for (i = 0; i < 6; i++) {
		if (changed == i)
			sprintf(print + strlen(print), "[%02x", hw_uart->data[i]);
		else if (changed == i - 1)
			sprintf(print + strlen(print), "]%02x", hw_uart->data[i]);
		else
			sprintf(print + strlen(print), " %02x", hw_uart->data[i]);
	}
	if (changed == 5)
		sprintf(print + strlen(print), "]\n");
	else
		sprintf(print + strlen(print), "\n");
}