#ifndef HWTTY_H
#define HWTTY_H

#include "sol1_cpu.h"
#include "hw_uart.h"

#if defined(__linux__) || defined(__MINGW32__)
#include <sys/socket.h>
#include <fcntl.h>
#else

#include <winsock2.h>
#include <ws2tcpip.h>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#endif

//#include <chrono>
//using namespace std::chrono;

		
	struct computer_client {

		int running;
		int index;
#ifdef _MSC_VER    
		SOCKET *client;
#else
		int *client;
#endif
		struct hw_uart* hw_uart;
		Queue* tty_out;

	};

class HW_TTY {

public:


	
	struct computer_client clients[10];

	struct net_data {
		SOL1_BYTE data;
	};

	//void init(SOL1_CPU& sol1_cpu, struct hw_uart *hw_uart);
	void start_server(SOL1_CPU& sol1_cpu, struct hw_uart* hw_uart);
	
};
#endif