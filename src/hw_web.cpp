#include "hw_web.h"
#include "sol1_computer.h"
#include <stdio.h>

#include <iostream>
using std::cout;
#include <fstream>
using std::ifstream;
using std::getline;
using std::string;




size_t count_lines(const char *filename)
{
	ifstream myfile(filename);
	string line;
	size_t count = 0;
	while (getline(myfile, line))
	{
		++count;
	}
	return count;
}

int line(string newline, int append)
{
	string newfile = "";

	if (append == 0) {
		const char filename[] = "html.cache";
		int i, count = count_lines(filename);
		ifstream myfile(filename);
		if (!myfile.fail()) {
			string line;

			for (i = 0; i < count - 80; ++i)
			{
				getline(myfile, line); /* read and discard: skip line */
			}
			while (getline(myfile, line))
			{
				newfile = newfile + line + "\n";
			}

			myfile.close();
		}
	}

	ofstream fout;
	if (append == 0)
		fout.open("html.cache");// , ios::app);
	else
		fout.open("html.cache", ios::app);

	if (append == 0)
		fout << newfile;
	fout << newline;

	//cout << "file saved" << endl;

	fout.flush();
	fout.close();

	return 0;
}


void HW_WEB::new_char(SOL1_BYTE c) {


	if (c == 0x0d) {
		this->currentline = c;
		line(this->currentline, 0);
		this->currentline = "";
	}
	else if (c != 0x0a) {
		this->currentline = c;
		line(this->currentline, 1);
		this->currentline = "";
	}

	//this->currentline = this->currentline + (char)c;
}

/*
void HW_WEB::init(SOL1_CPU& sol1_cpu, struct hw_uart* hw_uart) {
	int i;

	for (i = 0; i < 10; i++) {
		this->clients[i].client = NULL;
		this->clients[i].index = i;
		this->clients[i].running = 0;
		//this->clients[i].sol1_cpu = sol1_cpu;
		this->clients[i].hw_uart = hw_uart;
		this->clients[i].uart_out = queue_create();
	}

}
*/






DWORD WINAPI WebClientThread(LPVOID pParam)
{

	struct hw_web_client new_web_client = *(struct hw_web_client*)pParam;


	char welm[512];
	SOL1_BYTE buff[512];
	int n;
	int x = 0;
	BOOL auth = false;

	SOL1_BYTE lastchar = 0;
	SOL1_BYTE startCMD = 0x00;

	SOCKET client = *(new_web_client.client);


	while (true)
	{
		//printf("%02x -> %04x\n", new_computer_client->index, new_computer_client->client);

		n = recv(client, (char *)buff, 512, 0);// MSG_WAITALL);

		if (n > 0) {
			//if (RecvBytes > 0) {
			int i;

			string cmd = "";
			for (i = 0; i < n; i++)
			{
				cmd = cmd + (char)buff[i];
				/*
				if (buff[i] == 0x0d) {
					hw_uart_receive(new_web_client.hw_uart, buff[x]);
					break;
				}
				*/
			}

			if (cmd.find("cpu_command=") != std::string::npos)
			{
				string str3 = cmd.substr(12 + cmd.find("cpu_command="), cmd.length() - cmd.find("cpu_command="));
				int lll = str3.length();
				if (lll > 0) {
					const char *ccc = str3.c_str();

					for (i = 0; i < lll; i++) {
						hw_uart_receive(new_web_client.hw_uart, ccc[i]);
					}

					hw_uart_receive(new_web_client.hw_uart, '\r');
				}


			}


			n = 0;
			x = 0;
			memset(buff, 0, 512 * (sizeof buff[0]));

			string cur_response = "";
			ifstream myfile("html.cache");
			if (!myfile.fail()) {
				string line;
				while (getline(myfile, line))
				{
					cur_response = cur_response + line;
				}

				myfile.close();
			}
			cur_response = cur_response;

			string curline = "";
			curline = "HTTP/1.1 200 OK";
			send(client, curline.c_str(), curline.length(), 0);

			curline = "Content-Type: text/html\r\n";
			send(client, curline.c_str(), curline.length(), 0);

			curline = "Content-Length: " + to_string(cur_response.length()) + "\r\n";
			send(client, curline.c_str(), curline.length(), 0);

			curline = "Connection: close\r\n\r\n";
			send(client, curline.c_str(), curline.length(), 0);


			send(client, cur_response.c_str(), cur_response.length(), 0);
			//Content - Length: 12\r\n

			Sleep(1000);
			break;
		}
		else if (n == 0)
			break;

	}

	closesocket(client);
	((struct hw_web_client*)pParam)->running = 0;
	((struct hw_web_client*)pParam)->client = NULL;
	return 0;

}




DWORD WINAPI WebServerThread(LPVOID pParam)
{

	struct hw_web_client* clients = (struct hw_web_client*)pParam;

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
	local.sin_port = htons((u_short)20280);
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

		//u_long mode = 1;  // 1 to enable non-blocking socket
		//ioctlsocket(client, FIONBIO, &mode);


		struct hw_web_client *new_web_client = NULL;
		int i = 0;
		for (i = 0; i < 10; i++) {
			if (clients[i].running == 0) {

				clients[i].client = &client;
				clients[i].running = 1;
				new_web_client = &clients[i];
				break;
			}
		}
		if (new_web_client != NULL) {
			DWORD tid = 100 + new_web_client->index;
			HANDLE myHandle = CreateThread(0, 0, WebClientThread, new_web_client, 0, &tid);
		}
		else {

			char *buf_send = "No pool available.\n";
			send(client, buf_send, (int)strlen(buf_send), 0);
			closesocket(client);
		}
	}

	return 0;
}



void HW_WEB::start_server(SOL1_CPU& sol1_cpu, struct hw_uart* hw_uart) {
	DWORD tid;

	int i;

	for (i = 0; i < 10; i++) {
		this->clients[i].client = NULL;
		this->clients[i].index = i;
		this->clients[i].running = 0;
		//this->clients[i].sol1_cpu = sol1_cpu;
		this->clients[i].hw_uart = hw_uart;
		this->clients[i].web_out = queue_create();
	}

	HANDLE myHandle = CreateThread(0, 0, WebServerThread, &this->clients, 0, &tid);
	remove("html.cache");
}

