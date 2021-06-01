#include "hw_tty.h"
#include "sol1_computer.h"
#include <stdio.h>

#ifdef _MSC_VER    
#include <windows.h>
#else
#include <pthread.h> 
#endif



#ifdef _MSC_VER    
DWORD WINAPI TelnetClientThread(LPVOID pParam)
#else
void *TelnetClientThread(void *pParam)
#endif
{

	struct computer_client new_computer_client = *(struct computer_client*)pParam;


	char welm[512];
	SOL1_BYTE buff[512];
	int n;
	int x = 0;

	SOL1_BYTE lastchar = 0;
	SOL1_BYTE startCMD = 0x00;

#ifdef _MSC_VER    
	SOCKET client = *(new_computer_client.client);
#else
	int client = *(new_computer_client.client);
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
	sprintf(welm + strlen(welm), "terminal-%d initialized\r\n", (new_computer_client.index + 1));
	send(client, welm, (int)strlen(welm), 0);

	hw_uart_receive(new_computer_client.hw_uart, 0x0d);
	//memset(welm, 0, 512 * (sizeof buff[0]));


	while (true)
	{
		//printf("%02x -> %04x\n", new_computer_client->index, new_computer_client->client);

		if (!queue_empty(new_computer_client.tty_out)) {
			HW_TTY::net_data *data = (HW_TTY::net_data*)queue_remove(new_computer_client.tty_out);
			char buf_send[4];
			buf_send[0] = data->data;
			buf_send[1] = '\0';
			buf_send[2] = '\0';
			buf_send[3] = '\0';

			if (buf_send[0] == 0x08) { //CONVERT BS TO DEL
				buf_send[1] = 0x20;
				buf_send[2] = 0x08;
			}
			send(client, buf_send, (int)strlen(buf_send), 0);

		}





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

					hw_uart_receive(new_computer_client.hw_uart, buff[x]);
					//new_computer_client->sol1_cpu.microcode.mccycle.int_request = 0x01;
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
	((struct computer_client*)pParam)->running = 0;
	((struct computer_client*)pParam)->client = NULL;
	return 0;

}



#ifdef _MSC_VER    
DWORD WINAPI TelnetServerThread(LPVOID pParam)
#else
void *TelnetServerThread(void *pParam)
#endif
{

	struct computer_client* clients = (struct computer_client*)pParam;

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
	local.sin_port = htons((u_short)20248);
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
	if (bind(server, (sockaddr*)&local, sizeof(local)) != 0)
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

		struct computer_client *new_computer_client = NULL;
		int i = 0;
		for (i = 0; i < 10; i++) {
			if (clients[i].running == 0) {

				clients[i].client = &client;
				clients[i].running = 1;
				new_computer_client = &clients[i];
				break;
			}
		}
		if (new_computer_client != NULL) {


#ifdef _MSC_VER    
			DWORD tid = 100 + new_computer_client->index;
			HANDLE myHandle = CreateThread(0, 0, TelnetClientThread, new_computer_client, 0, &tid);
#else
			pthread_t tid = 100 + new_computer_client->index;
			pthread_create(&tid, NULL, TelnetClientThread, (void *)new_computer_client);
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



void HW_TTY::start_server(SOL1_CPU& sol1_cpu, struct hw_uart* hw_uart) {


	int i;

	for (i = 0; i < 10; i++) {
		this->clients[i].client = NULL;
		this->clients[i].index = i;
		this->clients[i].running = 0;
		//this->clients[i].sol1_cpu = sol1_cpu;
		this->clients[i].hw_uart = hw_uart;
		this->clients[i].tty_out = queue_create();

	}

#ifdef _MSC_VER        
	DWORD tid;
	HANDLE myHandle = CreateThread(0, 0, TelnetServerThread, &this->clients, 0, &tid);
#else
	pthread_t tid;
	pthread_create(&tid, NULL, TelnetServerThread, (void*)&this->clients);
#endif
}


