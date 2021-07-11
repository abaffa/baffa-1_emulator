//
// hw_uart.h
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
#ifndef HWUART_H
#define HWUART_H

#include <queue>

#if defined(__linux__) || defined(__MINGW32__)
#include <fcntl.h>
#else
#include <mutex> 
#endif

using namespace std;

class hw_uart {

public:

	//SOL1_BYTE status;
	
	queue<SOL1_BYTE> uart_in;
	queue<SOL1_BYTE> uart_out;
	SOL1_BYTE data[6];

#ifdef _MSC_VER    
	mutex mtx_out;
	condition_variable cv_out;

#else
	pthread_mutex_t mtx_out;
#endif

	void *sol1_cpu;

void init(void *sol1_cpu);

int read();
int write();

SOL1_BYTE get_lsr();


void send(SOL1_BYTE b);
void receive(SOL1_BYTE b);

void print(char *dir, int changed, char *print);
};

#endif
