// Clients.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <WinSock2.h>
#include <process.h>
#include <string>
#include <sstream>
#pragma comment(lib,"Ws2_32.lib")

struct TestData
{
	USHORT length;
	USHORT mainCmd;
	USHORT subCmd;
	int data;
	TestData()
	{
		mainCmd = 1;
		subCmd = 2;
		length = sizeof(TestData);
		data = 0;
	}
};

using namespace std;
const char* ip = "192.168.0.89";
uint16_t port = 22222;

SOCKET CreateSocket()
{
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
	Client() { s = INVALID_SOCKET; isRun = false; };
	~Client() {};

	bool ConnectToServer()
	{
		s = CreateSocket();
		if (s == INVALID_SOCKET) return false;
		isRun = true;
		return true;
	}
public:
	SOCKET s;
	bool isRun;
};


UINT __stdcall SendThread(LPVOID lParam)
{
	Client* pClient = (Client*)lParam;
	static UINT n = 0;
	TestData data;
	while (1)
	{
		if (!pClient->isRun)
		{
			Sleep(1);
			continue;
		}
		data.data = n;
		send(pClient->s, (char*)&data, sizeof(TestData), 0);
		Sleep(10000);
		n++;
	}
}

UINT __stdcall RecvThread(LPVOID lParam)
{
	Client* pClient = (Client*)lParam;
	char buf[1024];
	while (1)
	{
		if (!pClient->isRun)
		{
			Sleep(1);
			continue;
		}
		memset(buf, 0, 1024);
		recv(pClient->s, buf, 1024, 0);
		TestData* pData = (TestData*)buf;
		cout << "length: " << pData->length
			<< " mainCmd: " << pData->mainCmd
			<< " subCmd: " << pData->subCmd
			<< " data: " << pData->data << endl;
	}
}

int main()
{
	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data);
	Client* pClient = new Client();
	if (pClient->ConnectToServer())
	{
		HANDLE sendHandle = (HANDLE)_beginthreadex(NULL, 0, SendThread, pClient, 0, NULL);
		HANDLE recvHandle = (HANDLE)_beginthreadex(NULL, 0, RecvThread, pClient, 0, NULL);
		WaitForSingleObject(sendHandle, INFINITE);
		WaitForSingleObject(recvHandle, INFINITE);
	}
	return 0;
}