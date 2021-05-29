#ifndef HWRTC_H
#define HWRTC_H


#include "config.h"

struct hw_rtc{
	SOL1_BYTE data[16];
};

SOL1_DWORD hw_rtc_current_datetime();


void hw_rtc_init(struct hw_rtc* hw_rtc);
void hw_rtc_print(struct hw_rtc* hw_rtc, char *dir, int changed, char *print);

void hw_rtc_start_clock(struct hw_rtc* hw_rtc);

#endif
