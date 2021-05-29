#ifndef HWTIMER_H
#define HWTIMER_H


#include "config.h"

struct hw_timer {
	SOL1_BYTE data[4];
};

//SOL1_DWORD hw_timer_current_datetime();


void hw_timer_tick_c0(struct hw_timer* hw_timer);
void hw_timer_tick_c1(struct hw_timer* hw_timer);

void hw_timer_set_c1(struct hw_timer* hw_timer, SOL1_BYTE c);
void hw_timer_set_c0(struct hw_timer* hw_timer, SOL1_BYTE c);

void hw_timer_init(struct hw_timer* hw_timer);
void hw_timer_print(struct hw_timer* hw_timer, char *dir, int changed, char *print);

#endif
