//Client
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include<Windows.h>
#include<winsock2.h>
#include<Ws2tcpip.h>
#include<iphlpapi.h>
#include<stdio.h>
#include<iostream>
#include<iomanip>
#include<time.h>
using namespace std;

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 10240
#define DEFAULT_PORT "27015"

void generateNums(char str[DEFAULT_BUFLEN]);

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	char recvbuf[DEFAULT_BUFLEN] = {"\0"};
	int recvbuflen = DEFAULT_BUFLEN;
	int iResult;
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		cerr << "WSAStartup failed with error:" << iResult << endl;
		system("pause");
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
	if(iResult != 0) 
	{
		cerr <<"getaddrinfo failed with error: " <<  iResult;
		WSACleanup();
		system("pause");
		return 1;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			cerr << "socket failed with error: "<< WSAGetLastError();
			WSACleanup();
			system("pause");
			return 1;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		cout << "Unable to connect to server!\n";
		WSACleanup();
		system("pause");
		return 1;
	}
	clock_t ttime =0;
	// Receive until the peer closes the connection
	while (iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0) != SOCKET_ERROR)
	{
		//double res_time = ((double)end - start) / ((double)CLOCKS_PER_SEC)/2;
		cout << "Server > " << recvbuf << endl  << "Client > ";
		cin.getline(recvbuf, recvbuflen);
		if (!strcmp(recvbuf, "test"))
		{
			generateNums(recvbuf);
		}
		int sentbuflen = strlen(recvbuf);
		ttime = clock();
		send(ConnectSocket, recvbuf, recvbuflen, 0);
		recv(ConnectSocket, recvbuf, recvbuflen, 0);
		ttime = (clock() - ttime);
		cout << setprecision(10) << ttime << " sec\n";
		//_itoa_s((double)ttime / ((double)CLOCKS_PER_SEC), recvbuf, DEFAULT_BUFLEN, 10);
		recv(ConnectSocket, recvbuf, recvbuflen, 0);
		int resbuflen = atoi(recvbuf);
		cout << "Lost " << 100 -(resbuflen / sentbuflen * 100)<< "%\n";
		//send(ConnectSocket, "", recvbuflen, 0);
	}
	// cleanup
	closesocket(ConnectSocket);

	WSACleanup();

	system("pause");
	return 0;
}

void generateNums(char str[DEFAULT_BUFLEN])
{
	srand(time(NULL));
	for (int i = 0; i < DEFAULT_BUFLEN-1; i++)
	{
		str[i] = rand() % 127;
		if (str[i] < ' ')			//to avoid control characters
			str[i] += ' ';
	}
	str[DEFAULT_BUFLEN - 1] = '\0';
}

