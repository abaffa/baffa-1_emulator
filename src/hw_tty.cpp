#include "hw_tty.h"
#include "sol1_computer.h"
#include <stdio.h>



void HW_TTY::init(SOL1_CPU& sol1_cpu, struct hw_uart* hw_uart) {
	int i;
	long size = 0;

	for (i = 0; i < 10; i++) {
		this->clients[i].client = NULL;
		this->clients[i].index = i;
		this->clients[i].running = 0;
		//this->clients[i].sol1_cpu = sol1_cpu;
		this->clients[i].hw_uart = hw_uart;
		this->clients[i].uart_out = queue_create();
	}

}




DWORD WINAPI ClientThread(LPVOID pParam)
{

	struct computer_client *new_computer_client = (struct computer_client*)pParam;


	char welm[512];
	SOL1_BYTE buff[512];
	int n;
	int x = 0;
	BOOL auth = false;

	SOL1_BYTE lastchar = 0;
	SOL1_BYTE startCMD = 0x00;

	SOCKET *client = new_computer_client->client;// (SOCKET)pParam;

	//https://datatracker.ietf.org/doc/html/rfc854#page-14
	//https://datatracker.ietf.org/doc/html/rfc858
	//https://datatracker.ietf.org/doc/rfc930/
	send(*client, "\377\375\042\377\373\001\r\n", 6, 0);


	sprintf(welm, "Sol-1 74HC HomebrewCPU MiniComputer.\r\n");
	sprintf(welm + strlen(welm), "Emulator Version 0.01\r\n");
	sprintf(welm + strlen(welm), "\r\n");
	sprintf(welm + strlen(welm), "terminal-%d initialized\r\n", (new_computer_client->index + 1));
	send(*client, welm, (int)strlen(welm), 0);

	hw_uart_receive(new_computer_client->hw_uart, 0x0d);
	//memset(welm, 0, 512 * (sizeof buff[0]));


	while (true)
	{

		if (!queue_empty(new_computer_client->uart_out)) {
			HW_TTY::net_data *data = (HW_TTY::net_data*)queue_remove(new_computer_client->uart_out);
			char buf_send[4];
			buf_send[0] = data->data;
			buf_send[1] = '\0';
			buf_send[2] = '\0';
			buf_send[3] = '\0';

			if (buf_send[0] == 0x08) { //CONVERT BS TO DEL
				buf_send[1] = 0x20;
				buf_send[2] = 0x08;
			}
			send(*client, buf_send, (int)strlen(buf_send), 0);

		}





		if (x == 0)
			n = recv(*client, (char *)buff, 1, 0);// MSG_WAITALL);



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

					hw_uart_receive(new_computer_client->hw_uart, buff[x]);
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
	new_computer_client->running = 0;

	closesocket(*client);
	return 0;

}




DWORD WINAPI MTServerThread(LPVOID pParam)
{

	struct computer_client *clients = (struct computer_client*)pParam;

	SOCKET server;

	WSADATA wsaData;
	sockaddr_in local;
	int wsaret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (wsaret != 0)
	{
		return 0;
	}
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = INADDR_ANY;
	local.sin_port = htons((u_short)20248);
	server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server == INVALID_SOCKET)
	{
		return 0;
	}
	if (bind(server, (sockaddr*)&local, sizeof(local)) != 0)
	{
		return 0;
	}
	if (listen(server, 10) != 0)
	{
		return 0;
	}

	SOCKET client;
	sockaddr_in from;
	int fromlen = sizeof(from);

	while (true)
	{
		client = accept(server,
			(struct sockaddr*)&from, &fromlen);

		u_long mode = 1;  // 1 to enable non-blocking socket
		ioctlsocket(client, FIONBIO, &mode);


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
			DWORD tid;
			HANDLE myHandle = CreateThread(0, 0, ClientThread, new_computer_client, 0, &tid);
		}
		else {

			char *buf_send = "No pool available.\n";
			send(client, buf_send, (int)strlen(buf_send), 0);
			closesocket(client);
		}
	}

	return 0;
}



void HW_TTY::start_server() {
	DWORD tid;
	HANDLE myHandle = CreateThread(0, 0, MTServerThread, &this->clients, 0, &tid);
}

