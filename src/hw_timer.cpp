#include <stdio.h>
#include "hw_timer.h"
#include "utils.h"

#include <time.h>




void hw_timer_init(struct hw_timer* hw_timer) {
	int i = 0;
	for (i = 0; i < 4; i++) {
		hw_timer->data[i] = 0x00;
	}
}



void hw_timer_set_c0(struct hw_timer* hw_timer, SOL1_BYTE c) {
	hw_timer->data[0] = c;
	if (hw_timer->data[0] == 0xFF) {
		hw_timer->data[3] = hw_timer->data[3] | 0b00100000;
	}
	else if (hw_timer->data[0] == 0x00) {
		hw_timer->data[3] = hw_timer->data[3] & 0b11011111;
	}
}

void hw_timer_set_c1(struct hw_timer* hw_timer, SOL1_BYTE c) {
	hw_timer->data[1] = c;
	if (get_byte_bit(hw_timer->data[3], 6) == 0x01)
	{
		hw_timer->data[1]++;
		if (hw_timer->data[1] == 0xFF) {
			hw_timer->data[3] = hw_timer->data[3] | 0b10000000;
		}
		else if (hw_timer->data[1] == 0x00) {
			hw_timer->data[3] = hw_timer->data[3] & 0b01111111;
		}
	}
}


void hw_timer_tick_c0(struct hw_timer* hw_timer)
{
	if (get_byte_bit(hw_timer->data[3], 4) == 0x01)
	{
		hw_timer->data[0]++;
		if (hw_timer->data[0] == 0xFF) {
			hw_timer->data[3] = hw_timer->data[3] | 0b00100000;
		}
		else if (hw_timer->data[0] == 0x00) {
			hw_timer->data[3] = hw_timer->data[3] & 0b11011111;
		}
	}

}




void hw_timer_tick_c1(struct hw_timer* hw_timer)
{
	if (get_byte_bit(hw_timer->data[3], 6) == 0x01)
	{
		hw_timer->data[1]++;
		if (hw_timer->data[1] == 0xFF) {
			hw_timer->data[3] = hw_timer->data[3] | 0b10000000;
		}
		else if (hw_timer->data[1] == 0x00) {
			hw_timer->data[3] = hw_timer->data[3] & 0b01111111;
		}
	}

}


void hw_timer_print(struct hw_timer* hw_timer, char *dir, int changed, char *print) {


	int i = 0;
	sprintf(print, ">>> TIMER [%s]:", dir);
	for (i = 0; i < 4; i++) {
		if (changed == i)
			sprintf(print + strlen(print), "[%02x", hw_timer->data[i]);
		else if (changed == i - 1)
			sprintf(print + strlen(print), "]%02x", hw_timer->data[i]);
		else
			sprintf(print + strlen(print), " %02x", hw_timer->data[i]);
	}
	if (changed == 3)
		sprintf(print + strlen(print), "]\n");
	else
		sprintf(print + strlen(print), "\n");
}
