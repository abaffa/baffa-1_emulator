#ifndef HWTTY_H
#define HWTTY_H

#include "config.h"
#include "hw_uart.h"
#include "hw_tty_client.h"
#include <queue>

#if defined(__linux__) || defined(__MINGW32__)
#include <sys/socket.h>
#include <fcntl.h>
#else
#include <mutex> 
#include <winsock2.h>
#include <ws2tcpip.h>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#endif


using namespace std;




class HW_TTY {


public:

	HW_TTY_CLIENT clients[10];

	struct net_data {
		SOL1_BYTE data;
	};

	SOL1_BYTE started;
	SOL1_BYTE debug_call;
	SOL1_BYTE console;

	//void init(struct hw_uart *hw_uart);
	void start_server(struct hw_uart* hw_uart);

	queue<SOL1_BYTE> tty_in;

	void send(SOL1_BYTE b);
	SOL1_BYTE receive();

	void print(const char* s);

	void set_input(SOL1_BYTE b);

	char* gets(int max_value);
	char* getline();
	char get_char();

	HW_TTY() {
		started = 0;
		debug_call = 0;
		console = 0;
	}
};
#endif