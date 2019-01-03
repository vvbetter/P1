#pragma once
#include <WinSock2.h>
#include "ThreadPool.h"
#include <iostream>
using namespace std;

inline SOCKET CreateSocketClient()
{
	const char* ip = "120.79.242.138";
	uint16_t port = 12021;

	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == INVALID_SOCKET)
	{
		cout << "CreateSocket erroor" << WSAGetLastError() << endl;
		return INVALID_SOCKET;
	}
	sockaddr_in addr;
	addr.sin_addr.S_un.S_addr = inet_addr(ip);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	while (1)
	{
		int iResult = connect(s, (sockaddr*)&addr, sizeof(addr));
		if (iResult == SOCKET_ERROR)
		{
			cout << "Connect failed" << WSAGetLastError() << endl;
			Sleep(1000);
		}
		else
		{
			break;
		}
	}
	return s;
}

class Client
{
public:
	static UINT SendThread(LPVOID lParam);
	static UINT RecvThread(LPVOID lParam);
public:
	Client();;
	~Client();;

	bool ConnectToServer();
public:
	SOCKET s;
	bool isRun;
	volatile DWORD lastSendTime;
	volatile UINT nSend;
	volatile UINT64 nAverage;

	UINT recvTaskId;
	UINT sendTaskId;
};
