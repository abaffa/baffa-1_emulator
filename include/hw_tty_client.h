#ifndef HWTTYCLIENT_H
#define HWTTYCLIENT_H
#include "config.h"
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

class HW_TTY_CLIENT {

public:
	int running;
	int index;
#ifdef _MSC_VER    
	SOCKET *client;
	mutex mtx_out;
	condition_variable cv_out;

#else
	int *client;
	pthread_mutex_t mtx_out;
#endif
	struct hw_uart* hw_uart;
	queue<SOL1_BYTE> tty_out;
	queue<SOL1_BYTE>* tty_in;
	SOL1_BYTE* console;
	SOL1_BYTE* debug_call;

	HW_TTY_CLIENT() {
		tty_in = NULL;
	}
};

#endif