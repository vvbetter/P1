#include "pch.h"
#include <iostream>
#include <thread>
#include <WinSock2.h>
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "Ws2_32.lib")

void SendThread(SOCKET sendSocket, sockaddr_in RecvAddr)
{
	int n = 10000000;
	char buf[50] = { '1','2','3','4','5','6','7','8','9','0', '1','2','3','4','5','6','7','8','9','0', '1','2','3','4','5','6','7','8','9','0', '1','2','3','4','5','6','7','8','9','0', '1','2','3','4','5','6','7','8','9',0 };
	while (n--)
	{
		int nRst = sendto(sendSocket, buf, 50, 0, (SOCKADDR*)&RecvAddr, sizeof(RecvAddr));
		if (nRst == SOCKET_ERROR)
		{
			std::cout << "error: " << WSAGetLastError() << std::endl;
			break;
		}
		if (nRst != 50)
		{
			std::cout << "Êµ¼Ê·¢ËÍ n = " << nRst << std::endl;
			break;
		}
	}
}
int main(int argc,char* argv[])
{
	DWORD startime = timeGetTime();
	int iResult = 0;
	WSADATA wsaData;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		wprintf(L"WSAStartup failed with error %d\n", iResult);
		return 1;
	}
	SOCKET sendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	int sendbufsize = 50 * 1000000;
	setsockopt(sendSocket, SOL_SOCKET, SO_SNDBUF, (char*)&sendbufsize, sizeof(int));
	USHORT Port = 50001;
	sockaddr_in RecvAddr;
	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_port = htons(Port);
	RecvAddr.sin_addr.s_addr = inet_addr("192.168.0.89");
	
	std::thread t1(std::bind(SendThread, sendSocket, RecvAddr));
	std::thread t2(std::bind(SendThread, sendSocket, RecvAddr));
	t1.join();
	t2.join();

	std::cout << "send over " << timeGetTime() - startime << std::endl;
	closesocket(sendSocket);
	WSACleanup();
	return 0;
}