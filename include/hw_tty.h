#ifndef HWTTY_H
#define HWTTY_H

#include "sol1_cpu.h"
#include "hw_uart.h"

#include <winsock2.h>
#include <ws2tcpip.h>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


		
	struct computer_client {

		int running;
		int index;
		SOCKET *client;
		struct hw_uart* hw_uart;
		Queue* tty_out;

	};

class HW_TTY {

public:


	
	struct computer_client clients[10];

	struct net_data {
		SOL1_BYTE data;
	};

	//void HW_TTY::init(SOL1_CPU& sol1_cpu, struct hw_uart *hw_uart);
	void HW_TTY::start_server(SOL1_CPU& sol1_cpu, struct hw_uart* hw_uart);
	
};
#endif