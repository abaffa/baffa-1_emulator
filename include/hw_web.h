#ifndef HWWEB_H
#define HWWEB_H

#include "sol1_cpu.h"
#include "hw_uart.h"

#include <winsock2.h>
#include <ws2tcpip.h>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#include <string>



struct hw_web_client {

	int running;
	int index;
	SOCKET *client;
	struct hw_uart* hw_uart;
	Queue* web_out;

};

class HW_WEB {

public:


	std::string currentline;

	struct hw_web_client clients[10];

	struct net_data {
		SOL1_BYTE data;
	};

	HW_WEB() {
		this->currentline = "";
	}
	//void HW_TTY::init(SOL1_CPU& sol1_cpu, struct hw_uart *hw_uart);
	void HW_WEB::new_char(SOL1_BYTE c);
	void HW_WEB::start_server(SOL1_CPU& sol1_cpu, struct hw_uart* hw_uart);

};
#endif