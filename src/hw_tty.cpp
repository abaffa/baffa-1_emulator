#include "hw_tty.h"
#include "sol1_computer.h"
#include <stdio.h>
#include <utils.h>

#ifdef _MSC_VER    
#include <windows.h>
#include <conio.h>
#else
#include <pthread.h> 
#endif




#ifdef _MSC_VER    
DWORD WINAPI TelnetClientThread(LPVOID pParam)
#else
void *TelnetClientThread(void *pParam)
#endif
{
	
	HW_TTY_CLIENT *current_client = (HW_TTY_CLIENT*)pParam;


	char welm[512];
	SOL1_BYTE buff[512];
	int n;
	int x = 0;

	SOL1_BYTE lastchar = 0;
	SOL1_BYTE startCMD = 0x00;

#ifdef _MSC_VER    
	SOCKET client = *(current_client)->client;
#else
	int client = *(current_client)->client;
#endif

	//https://datatracker.ietf.org/doc/html/rfc1116
	//https://tools.ietf.org/html/rfc854
	//https://datatracker.ietf.org/doc/html/rfc854#page-14
	//https://tools.ietf.org/html/rfc855
	//https://datatracker.ietf.org/doc/html/rfc858
	//https://datatracker.ietf.org/doc/rfc930/
	//https://en.wikipedia.org/wiki/Telnet#Related_RFCs
	//http://www.iana.org/assignments/telnet-options/telnet-options.xhtml
	//ByteString(-1, -5, 31, -1, -5, 32, -1, -5, 24, -1, -5, 39, -1, -3, 1, -1, -5, 3, -1, -3, 3),
	//ByteString(-1, -2, 31, -1, -2, 32, -1, -2, 24, -1, -2, 39, -1, -4, 1),
	//ByteString(-1, -5, 36),
	//ByteString(-1, -2, 36)
	//-1 -5 31 = IAC WILL NAWS
	//-1 -5 32 = IAC WILL TERMINAL-SPEED
	//-1 -5 24 = IAC WILL TERMINAL-TYPE
	//-1 -5 39 = IAC WILL NEW-ENVIRON
	//-1 -3 1  = IAC DO   ECHO
	//-1 -5 3  = IAC WILL SUPPRESS-GO-AHEAD
	//-1 -3 3  = IAC DO   SUPPRESS-GO-AHEAD
	//-1 -2 31 = IAC DONT NAWS
	//-1 -2 32 = IAC DONT TERMINAL-SPEED
	//-1 -2 24 = IAC DONT TERMINAL-TYPE
	//-1 -2 39 = IAC DONT NEW-ENVIRON
	//-1 -4 1  = IAC WONT ECHO
	//-1 -5 36 = IAC WILL ENVIRON
	//-1 -2 36 = IAC DONT ENVIRON
	send(client, "\377\375\042\377\373\001\r\n", 6, 0);


	sprintf(welm, "Sol-1 74HC HomebrewCPU MiniComputer.\r\n");
	sprintf(welm + strlen(welm), "Emulator Version 0.01\r\n");
	sprintf(welm + strlen(welm), "\r\n");
	sprintf(welm + strlen(welm), "terminal-%d initialized\r\n", (current_client->index + 1));
	send(client, welm, (int)strlen(welm), 0);

	//hw_uart_receive(current_client->hw_uart, 0x0d);
	//memset(welm, 0, 512 * (sizeof buff[0]));

	char lastSentChar = '\0';

	while (true)
	{
		//sprintf("%02x -> %04x\n", current_client->index, current_client->client);



		if (!current_client->tty_out.empty()) {


#ifdef _MSC_VER    
			std::unique_lock<std::mutex> lock(current_client->mtx_out);
#else
			pthread_mutex_lock(&current_client->mtx_out);
#endif

			SOL1_BYTE data = current_client->tty_out.front(); current_client->tty_out.pop();

#ifdef _MSC_VER    
			current_client->cv_out.notify_all();
#else
			pthread_mutex_unlock(&current_client->mtx_out);
#endif

			char buf_send[4];
			buf_send[0] = data;
			buf_send[1] = '\0';
			buf_send[2] = '\0';
			buf_send[3] = '\0';

			if (buf_send[0] == 0x08) { //CONVERT BS TO DEL
				buf_send[1] = 0x20;
				buf_send[2] = 0x08;
			}

			if (lastSentChar == '\r' && data != '\n')
				send(client, "\n", 1, 0);

			send(client, buf_send, (int)strlen(buf_send), 0);

			if (lastSentChar == '\n' &&  data != '\n')
				lastSentChar = '\0';
			else
				lastSentChar = data;

		}
		//std::unique_lock<std::mutex> unlock(current_client->mtx_out);




		if (x == 0)
			n = recv(client, (char *)buff, 1, 0);// MSG_WAITALL);



		if (n > 0) {
			//if (RecvBytes > 0) {

			if (x < n) {

				if (buff[x] == 0xFF) {
					startCMD = 0x01;
					lastchar = buff[x];
				}
				else if (startCMD == 0x01 && buff[x] == 0xFA) {
					startCMD = 0x02;
					lastchar = buff[x];
				}
				else if (startCMD == 0x02 && buff[x] == 0xF0) {
					startCMD = 0x00;
					lastchar = buff[x];
				}
				else if (startCMD == 0x01 && lastchar == 0xFF) {
					lastchar = buff[x];
				}
				else if (startCMD == 0x01) {
					startCMD = 0x00;
					lastchar = buff[x];
				}
				else if (startCMD == 0x00 && buff[x] != 0x00 && buff[x] != 0x0a) {
					if (buff[x] == 0x7F) //CONVERT DEL TO BS
						buff[x] = 0x08;

					if (*(((HW_TTY_CLIENT*)pParam)->console) == 0) {
						if (buff[x] == 0x04)
						{
							*(((HW_TTY_CLIENT*)pParam)->debug_call) = 1;
						}
						else {
							current_client->hw_uart->receive(buff[x]);
							//current_client->sol1_cpu.microcode.mccycle.int_request = 0x01;
						}
					}
					else if (*(((HW_TTY_CLIENT*)pParam)->console) == 1) {
						HW_TTY::net_data *net_data = (HW_TTY::net_data*)malloc(sizeof(HW_TTY::net_data));
						current_client->tty_in->push(buff[x]);
					}
				}
				x++;
			}
			else {
				n = 0;
				x = 0;
				memset(buff, 0, 512 * (sizeof buff[0]));
			}

		}
		else if (n == 0)
			break;

	}

#ifdef _MSC_VER     
	closesocket(client);
#else
	shutdown(client, 2);
#endif
	current_client->running = 0;
	current_client->client = NULL;
	return 0;

}



#ifdef _MSC_VER    
DWORD WINAPI TelnetServerThread(LPVOID pParam)
#else
void *TelnetServerThread(void *pParam)
#endif
{

	HW_TTY_CLIENT* clients = (HW_TTY_CLIENT*)pParam;

	sockaddr_in local;
#ifdef _MSC_VER    
	SOCKET server;
	WSADATA wsaData;
	int wsaret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (wsaret != 0)
	{
		return 0;
	}
#else
	int server;
#endif
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = INADDR_ANY;
	local.sin_port = htons((u_short)SERVER_PORT);
#ifdef _MSC_VER   
	server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server == INVALID_SOCKET)
	{
		return 0;
	}
#else
	if ((server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		return 0;
	}
#endif
	if (::bind(server, (sockaddr*)&local, sizeof(local)) != 0)
	{
		return 0;
	}
	if (listen(server, 10) != 0)
	{
		return 0;
	}

	sockaddr_in from;
#ifdef _MSC_VER    
	SOCKET client;
	int fromlen = sizeof(from);
#else
	int client;
	socklen_t fromlen = sizeof(from);
#endif

	while (true)
	{
		client = accept(server,
			(struct sockaddr*)&from, &fromlen);

#ifdef _MSC_VER    
		u_long mode = 1;  // 1 to enable non-blocking socket
		ioctlsocket(client, FIONBIO, &mode);
#else
		fcntl(client, F_SETFL, O_NONBLOCK);
#endif

		HW_TTY_CLIENT *new_client = NULL;
		int i = 0;
		for (i = 0; i < 10; i++) {
			if (clients[i].running == 0) {

				clients[i].client = &client;
				clients[i].running = 1;
				new_client = &clients[i];
				break;
			}
		}
		if (new_client != NULL) {


#ifdef _MSC_VER    
			DWORD tid = 100 + new_client->index;
			HANDLE myHandle = CreateThread(0, 0, TelnetClientThread, new_client, 0, &tid);
#else
			pthread_t tid = 100 + new_client->index;
			pthread_create(&tid, NULL, TelnetClientThread, (void *)new_client);
#endif


		}
		else {

			char *buf_send = (char *)"No pool available.\n";
			send(client, buf_send, (int)strlen(buf_send), 0);
#ifdef _MSC_VER     
			closesocket(client);
#else
			shutdown(client, 2);
#endif

		}
	}

	return 0;
}



void HW_TTY::start_server(struct hw_uart* hw_uart) {


	int i;
	//this->tty_in = queue_create();

	for (i = 0; i < 10; i++) {
		this->clients[i].client = NULL;
		this->clients[i].index = i;
		this->clients[i].running = 0;
		//this->clients[i].sol1_cpu = sol1_cpu;
		this->clients[i].hw_uart = hw_uart;
		//this->clients[i].tty_out = queue_create();
		//this->clients[i].mtx = &this->mtx;
		this->clients[i].tty_in = &this->tty_in;//queue_create();
		this->clients[i].console = &this->console;
		this->clients[i].debug_call = &this->debug_call;
	}

#ifdef _MSC_VER        
	DWORD tid;
	HANDLE myHandle = CreateThread(0, 0, TelnetServerThread, &this->clients, 0, &tid);
#else
	pthread_t tid;
	pthread_create(&tid, NULL, TelnetServerThread, (void*)&this->clients);
#endif

	this->started = 1;
}



void HW_TTY::send(SOL1_BYTE b) {

	if (this->started == 1) {
		int i;
		for (i = 0; i < 10; i++) {
			if (this->clients[i].running == 1) {  // SEND TELNET
				SOL1_BYTE data = b;
#ifdef _MSC_VER    
				std::unique_lock<std::mutex> lock(this->clients[i].mtx_out);
#else
				pthread_mutex_lock(&this->clients[i].mtx_out);
#endif
				this->clients[i].tty_out.push(data);

#ifdef _MSC_VER    
				this->clients[i].cv_out.notify_all();
#else
				pthread_mutex_unlock(&this->clients[i].mtx_out);
#endif
			}
		}
	}
}

void HW_TTY::print(const char* s) {

	if (this->started == 1) {
		int i = 0;
		while (s[i] != '\0') {
			if (s[i] == '\n') {
				send('\r');
				send(s[i]);
			}
			else
				send(s[i]);
			i++;
		}
	}
	printf("%s", s);
}





void HW_TTY::set_input(SOL1_BYTE b) {

	/*
	if (this->started == 1) {
		int i;
		for (i = 0; i < 10; i++) {
			if (clients[i].running == 1) {  // SEND TELNET
				clients[i].console = b;
			}
		}
	}
	*/
	this->console = b;
}

char* HW_TTY::gets(int max_value) {

	char str_out[255];
	char *input = (char*)malloc(sizeof(char) * 257);

	int i = 0;
	for (i = 0; i < 256 && i < max_value; ) {
		char cur_input = get_char();
		if (cur_input == (char)8) {
			if (i > 0) {
				sprintf(str_out, "%c", cur_input);
				sprintf(str_out, " ");
				sprintf(str_out, "%c", cur_input);
				print(str_out);
				i--;
			}
		}
		else if (cur_input != '\n' && cur_input != '\r') {
			cur_input = toupper(cur_input);
			sprintf(str_out, "%c", cur_input);
			print(str_out);
			input[i] = cur_input;
			i++;
		}
		else {
			print("\r\n");
			break;
		}
	}
	input[i] = '\0';

	return input;
}

char* HW_TTY::getline() {
	char str_out[255];
	char *input = (char*)malloc(sizeof(char) * 257);

	int i = 0;
	for (i = 0; i < 256; ) {
		char cur_input = get_char();
		if (cur_input == (char)8) {
			if (i > 0) {
				sprintf(str_out, "%c", cur_input);
				sprintf(str_out, " ");
				sprintf(str_out, "%c", cur_input);
				print(str_out);
				i--;
			}
		}
		else if (cur_input != '\n' && cur_input != '\r') {
			cur_input = toupper(cur_input);
			sprintf(str_out, "%c", cur_input);
			print(str_out);
			input[i] = cur_input;
			i++;
		}
		else {
			print("\r\n");
			break;
		}
	}
	input[i] = '\0';

	return input;
}


SOL1_BYTE HW_TTY::receive() {

	SOL1_BYTE ch = 0x00;
	set_input(1);
	while (1) {
		if (kbhit()) {
			ch = getch();
			break;
		}
		else if (!this->tty_in.empty()) {

			SOL1_BYTE data = this->tty_in.front(); this->tty_in.pop();
			ch = data;
			break;
		}

#ifdef _MSC_VER     
		Sleep(10);
#else
		int milliseconds = 10;
		struct timespec ts;
		ts.tv_sec = milliseconds / 1000;
		ts.tv_nsec = (milliseconds % 1000) * 1000000;
		nanosleep(&ts, NULL);

#endif
	}
	set_input(0);

	return ch;
}


char HW_TTY::get_char() {

	char cur_input = receive();
	return cur_input;
}