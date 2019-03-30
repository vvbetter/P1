// Clients.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include "Clients.h"

Client::Client()
{
	nSend = 0;
	nAverage = 0;
	lastSendTime = timeGetTime();
}

Client::~Client()
{
}

bool Client::ConnectToServer(char* ip, int port)
{
	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == INVALID_SOCKET)
	{
		cout << "CreateSocket erroor" << WSAGetLastError() << endl;
		return false;
	}
	sockaddr_in addr;
	addr.sin_addr.S_un.S_addr = inet_addr(ip);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	int iResult = connect(s, (sockaddr*)&addr, sizeof(addr));
	if (iResult == SOCKET_ERROR)
	{
		cout << "Connect failed" << WSAGetLastError() << endl;
		return false;
	}
	return true;
}

