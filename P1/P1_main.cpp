#include <iostream>
#include <map>
#include <vector>
#include <thread>
#include <WinSock2.h>
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "Ws2_32.lib")
using namespace std;
USHORT Port = 50001;
//fd_set* fds = new fd_set();
vector<SOCKET> RecvSockets;

void WorkThread(sockaddr_in clientAddr)
{
	char RecvBuf[1024] = { '1',0 };
	int BufLen = 1024;
	int iResult;
	int addrSize = sizeof(clientAddr);
	SOCKET RecvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	RecvSockets.push_back(RecvSocket);
	unsigned long arg = 1;
	setsockopt(RecvSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&arg, sizeof(arg));
	// Bind the socket to any address and the specified port.
	sockaddr_in RecvAddr;
	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_port = htons(Port);
	RecvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	iResult = ::bind(RecvSocket, (SOCKADDR *)& RecvAddr, sizeof(RecvAddr));
	if (iResult != 0) {
		cout << "Work socket bind failed with error :" << WSAGetLastError() << endl;
		closesocket(RecvSocket);
		return;
	}
	iResult = connect(RecvSocket, (SOCKADDR*)&clientAddr, sizeof(clientAddr));
	if (iResult == SOCKET_ERROR) {
		cout << "work thread connect error: " << WSAGetLastError() << endl;
		closesocket(RecvSocket);
		return;
	}
	cout << "work " << RecvSocket << " start" << endl;
	while (1)
	{
		send(RecvSocket, RecvBuf, strlen(RecvBuf), 0);

		recvfrom(RecvSocket, RecvBuf, BufLen, 0, (SOCKADDR *)& clientAddr, &addrSize);
		cout << "work " << RecvSocket << " recv data:" << RecvBuf << endl;
	}
}
vector<char[50]> g_data(1000000);
void ListenThread(SOCKET RecvSocket)
{
	char RecvBuf[50] = {0};
	int BufLen = 50;
	volatile int n = 0;

	sockaddr_in SenderAddr;
	int SenderAddrSize = sizeof(SenderAddr);

	while (1)
	{
		memset(RecvBuf, 0, BufLen);
		int iResult = recvfrom(RecvSocket,
			RecvBuf, BufLen, 0, (SOCKADDR *)& SenderAddr, &SenderAddrSize);
		if (iResult == SOCKET_ERROR) {
			cout << "listen socket recv from error: " << WSAGetLastError() << endl;
		}
		else
		{
			//cout << "listen socket Recvfrom Data:" << RecvBuf << endl;
			//thread* t = new thread(std::bind(WorkThread, SenderAddr));
			//t->detach();
			//sendto(RecvSocket, RecvBuf, strlen(RecvBuf), 0, (SOCKADDR*)&SenderAddr, SenderAddrSize);
			n++;
			if (n == 1000000)
			{
				std::cout <<"end time :"<< timeGetTime() << endl;
			}
			if (n % 100000 == 0)
			{
				std::cout << "n time :" << timeGetTime() << " # " << n << endl;
			}
		}
	}
}

void Listen2Thread(SOCKET RecvSocket)
{
	char RecvBuf[1024] = { 0 };
	int BufLen = 1024;
	int iResult;

	sockaddr_in SenderAddr;
	int SenderAddrSize = sizeof(SenderAddr);

	while (1)
	{
		memset(RecvBuf, 0, BufLen);
		int iResult = recvfrom(RecvSocket,
			RecvBuf, BufLen, 0, (SOCKADDR *)& SenderAddr, &SenderAddrSize);
		if (iResult == SOCKET_ERROR) {
			cout << "listen socket recv from error: " << WSAGetLastError() << endl;
		}
		else
		{
			cout << "listen2 socket Recvfrom Data:" << RecvBuf << endl;
			//thread* t = new thread(std::bind(WorkThread, SenderAddr));
			//t->detach();
			sendto(RecvSocket, RecvBuf, strlen(RecvBuf), 0, (SOCKADDR*)&SenderAddr, SenderAddrSize);
		}
	}
}


int main(int argc, char* argv[])
{
	int iResult = 0;

	std::cout << "start time :" << timeGetTime() << endl;


	WSADATA wsaData;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		wprintf(L"WSAStartup failed with error %d\n", iResult);
		return 1;
	}
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		wprintf(L"WSAStartup failed with error %d\n", iResult);
		return 1;
	}

	SOCKET RecvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	int recvBufSize = 500 * 1000000;
	setsockopt(RecvSocket, SOL_SOCKET, SO_RCVBUF, (char*)&recvBufSize, sizeof(int));
	RecvSockets.push_back(RecvSocket);
	unsigned long arg = 1;
	setsockopt(RecvSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&arg, sizeof(arg));
	// Bind the socket to any address and the specified port.
	sockaddr_in RecvAddr;
	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_port = htons(Port);
	RecvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	iResult = ::bind(RecvSocket, (SOCKADDR *)& RecvAddr, sizeof(RecvAddr));
	if (iResult != 0) {
		cout << "listen socket bind failed with error :" << WSAGetLastError() << endl;
	}

	thread t(std::bind(ListenThread, RecvSocket));
	t.join();
	WSACleanup();
	return 0;
}

