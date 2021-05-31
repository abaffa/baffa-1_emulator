#include <stdio.h>
#include "hw_rtc.h"
#include <windows.h>
#include <time.h>
#include <ctime>



SOL1_DWORD hw_rtc_current_datetime() {

	time_t timer;
	struct tm y2k = { 0 };
	SOL1_DWORD seconds;

	y2k.tm_hour = 0;   y2k.tm_min = 0; y2k.tm_sec = 0;
	y2k.tm_year = 100; y2k.tm_mon = 0; y2k.tm_mday = 1;

	time(&timer);  /* get current time; same as: timer = time(NULL)  */

	seconds = difftime(timer, mktime(&y2k));

	return seconds;
}



void hw_rtc_init(struct hw_rtc* hw_rtc) {
	int i = 0;
	for (i = 0; i < 16; i++) {
		hw_rtc->data[i] = 0x00;
	}
}

void hw_rtc_print(struct hw_rtc* hw_rtc, char *dir, int changed, char *print) {


	int i = 0;
	sprintf(print, ">>> RTC [%s]:", dir);
	for (i = 0; i < 16; i++) {
		if (changed == i)
			sprintf(print + strlen(print), "[%02x", hw_rtc->data[i]);
		else if (changed == i - 1)
			sprintf(print + strlen(print), "]%02x", hw_rtc->data[i]);
		else
			sprintf(print + strlen(print), " %02x", hw_rtc->data[i]);
	}
	if (changed == 15)
		sprintf(print + strlen(print), "]\n");
	else
		sprintf(print + strlen(print), "\n");
}



SOL1_BYTE convertToHex(int v) {
	char buffer[5];
	SOL1_BYTE ret = strtol(itoa(v, buffer, 10), NULL, 16);
	return ret;
}

SOL1_BYTE convertToInt(int v) {
	char buffer[5];
	SOL1_BYTE ret = strtol(itoa(v, buffer, 16), NULL, 10);
	return ret;
}

DWORD WINAPI MTRtcThread(LPVOID pParam)
{
	/*
	; RTC SERVICES INTERRUPT
		; RTC I / O bank = FFA0 to FFAF
		; FFA0 to FFA7 is scratch RAM
		; control register at $FFA8[W | R | S | Cal4..Cal0]
		; al = 0..6->get
		; al = 7..D->set
*/
	struct hw_rtc* hw_rtc = (struct hw_rtc*)pParam;
	struct tm * timeinfo;
	time_t rawtime;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	hw_rtc->data[15] = convertToHex(timeinfo->tm_year - 100);
	hw_rtc->data[14] = convertToHex(timeinfo->tm_mon);
	hw_rtc->data[13] = convertToHex(timeinfo->tm_mday);
	hw_rtc->data[12] = convertToHex(timeinfo->tm_wday);

	hw_rtc->data[11] = convertToHex(timeinfo->tm_hour);
	hw_rtc->data[10] = convertToHex(timeinfo->tm_min);
	hw_rtc->data[9] = convertToHex(timeinfo->tm_sec);
	
	while (true)
	{
		
		timeinfo->tm_year = convertToInt(hw_rtc->data[15]) + 100;
		timeinfo->tm_mon = convertToInt(hw_rtc->data[14]);
		timeinfo->tm_mday = convertToInt(hw_rtc->data[13]);

		timeinfo->tm_hour = convertToInt(hw_rtc->data[11]);
		timeinfo->tm_min = convertToInt(hw_rtc->data[10]);
		timeinfo->tm_sec = convertToInt(hw_rtc->data[9])+1;

		mktime(timeinfo);

		hw_rtc->data[15] = convertToHex(timeinfo->tm_year -100);
		hw_rtc->data[14] = convertToHex(timeinfo->tm_mon);
		hw_rtc->data[13] = convertToHex(timeinfo->tm_mday);
		hw_rtc->data[12] = convertToHex(timeinfo->tm_wday);

		hw_rtc->data[11] = convertToHex(timeinfo->tm_hour);
		hw_rtc->data[10] = convertToHex(timeinfo->tm_min);
		hw_rtc->data[9] = convertToHex(timeinfo->tm_sec);
		
		Sleep(1000);
	}

	return 0;
}





void hw_rtc_start_clock(struct hw_rtc* hw_rtc) {
	DWORD tid;
	HANDLE myHandle = CreateThread(0, 0, MTRtcThread, hw_rtc, 0, &tid);
}