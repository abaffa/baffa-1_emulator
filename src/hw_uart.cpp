//
// hw_uart.cpp
//
////// BEGIN LICENSE NOTICE//////
//
//Sol-1 HomebrewCPU Minicomputer System Emulator
//
//Copyright(C) 2021 Augusto Baffa, (sol-1.baffasoft.com.br)
//
//This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110 - 1301, USA.
//
////// END LICENSE NOTICE//////
//
#include <stdlib.h>
#include "sol1_cpu.h"
#include "utils.h"
#include "hw_uart.h"

#ifdef _MSC_VER    
#include <windows.h>
#else
#include <pthread.h> 
#include "utils.h"
#endif

void hw_uart::init(void *sol1_cpu) {

	//this->uart_in = queue_create();
	//this->uart_out = queue_create();
	this->data[5] = 0xFF;

	//this->status = 0x00;

	this->sol1_cpu = sol1_cpu;
}

int hw_uart::write() {

	if (!this->uart_in.empty()) {
		this->data[0] = this->uart_in.front(); this->uart_in.pop();
		printf("%c", this->data[0]);

		return 1;
	}

	return 0;
}

int hw_uart::read() {


	if (!this->uart_out.empty() && ((SOL1_CPU*)this->sol1_cpu)->microcode.controller_bus.int_request == 0x01) {
#ifdef _MSC_VER    
		std::unique_lock<std::mutex> lock(this->mtx_out);
#else
		pthread_mutex_lock(&this->mtx_out);
#endif
		this->data[0] = this->uart_out.front();  this->uart_out.pop();

		((SOL1_CPU*)this->sol1_cpu)->microcode.controller_bus.int_request = 0;
		

#ifdef _MSC_VER    
		this->cv_out.notify_all();
#else
		pthread_mutex_unlock(&this->mtx_out);
#endif


		return 1;
	}
	return 0;
}

SOL1_BYTE hw_uart::get_lsr() {

	if ((this->data[5] & 0x20) == 0x00) {
		this->data[5] = 0x20;

		if (!this->uart_in.empty())
			this->data[5] |= 0x80;

		if (!this->uart_out.empty())
			this->data[5] |= 0x40;

	}
	else
		this->data[5] = (this->data[5] & 0xDF) | ~(this->data[5] & 0x20);

	return this->data[5];
}


void hw_uart::receive(SOL1_BYTE data) {

#ifdef _MSC_VER    
	std::unique_lock<std::mutex> lock(this->mtx_out);
#else
	pthread_mutex_lock(&this->mtx_out);
#endif
	this->uart_out.push(data);


	((SOL1_CPU*)this->sol1_cpu)->microcode.controller_bus.int_req = 0xFF;
	((SOL1_CPU*)this->sol1_cpu)->microcode.controller_bus.int_vector = 0x07 << 1;
	((SOL1_CPU*)this->sol1_cpu)->microcode.controller_bus.int_request = 0x01;


#ifdef _MSC_VER    
	this->cv_out.notify_all();
#else
	pthread_mutex_unlock(&this->mtx_out);
#endif

}



void hw_uart::send(SOL1_BYTE data) {
	this->uart_in.push(data);
}


void hw_uart::print(char *dir, int changed, char *print) {

	int i = 0;
	sprintf(print, ">>> UART [%s]:", dir);
	for (i = 0; i < 6; i++) {
		if (changed == i)
			sprintf(print + strlen(print), "[%02x", this->data[i]);
		else if (changed == i - 1)
			sprintf(print + strlen(print), "]%02x", this->data[i]);
		else
			sprintf(print + strlen(print), " %02x", this->data[i]);
	}
	if (changed == 5)
		sprintf(print + strlen(print), "]\n");
	else
		sprintf(print + strlen(print), "\n");
}

