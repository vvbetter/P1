#include "pch.h"
#include <iostream>
#include <WinSock2.h>
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "Ws2_32.lib")
int main(int argc,char* argv[])
{
	int iResult = 0;
	WSADATA wsaData;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		wprintf(L"WSAStartup failed with error %d\n", iResult);
		return 1;
	}
	SOCKET sendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	int n = 1000100;
	USHORT Port = 50001;
	sockaddr_in RecvAddr;
	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_port = htons(Port);
	RecvAddr.sin_addr.s_addr = inet_addr("192.168.0.89");

	char buf[50] = { '1','2','3','4','5','6','7','8','9','0', '1','2','3','4','5','6','7','8','9','0', '1','2','3','4','5','6','7','8','9','0', '1','2','3','4','5','6','7','8','9','0', '1','2','3','4','5','6','7','8','9',0 };

	while (n--)
	{
		 int nRst = sendto(sendSocket, buf, 50, 0, (SOCKADDR*)&RecvAddr, sizeof(RecvAddr));
		 if (nRst == SOCKET_ERROR)
		 {
			 std::cout << "error: " << WSAGetLastError() << std::endl;
		 }
	}
	std::cout << "send over " << timeGetTime() << std::endl;
	closesocket(sendSocket);
	WSACleanup();
	return 0;
}