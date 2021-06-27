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

	SOL1_BYTE status;
	
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
