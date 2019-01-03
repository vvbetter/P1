// Clients.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include "Clients.h"

struct TestData
{
	USHORT length;
	USHORT mainCmd;
	USHORT subCmd;
	UINT data;
	DWORD sendTime;
	DWORD recvTime;
	TestData()
	{
		mainCmd = 0;
		subCmd = 1;
		length = sizeof(TestData);
		data = 0;
		sendTime = 0;
		recvTime = 0;
	}
};

UINT Client::SendThread(LPVOID lParam)
{
	Client* pClient = (Client*)lParam;
	DWORD tick = timeGetTime();
	if (tick - pClient->lastSendTime < 10)
	{
		return 0;
	}
	TestData tdata;
	tdata.data = pClient->nSend;
	tdata.sendTime = tick;
	int ret = send(pClient->s, (char*)&tdata, sizeof(TestData), 0);
	if (ret == SOCKET_ERROR)
	{
		cout << "Socket " << pClient->s << " send Error :" << WSAGetLastError() << endl;
		ThreadPool::GetInstance()->RemoveThreadTask(pClient->sendTaskId);
		return 0;
	}
	++pClient->nSend;
	if (pClient->nSend % 1000 == 0)
	{
		cout << "SOCKET " << pClient->s << " send data " << pClient->nSend << endl;
	}
	pClient->lastSendTime = tick;
	return 0;
}

UINT Client::RecvThread(LPVOID lParam)
{
	Client* pClient = (Client*)lParam;
	char buf[1024];
	{
		memset(buf, 0, 1024);
		int ret = recv(pClient->s, buf, 1024, 0);
		if (ret == SOCKET_ERROR)
		{
			cout << "Socket " << pClient->s << " recv Error :" << WSAGetLastError() << endl;
			ThreadPool::GetInstance()->RemoveThreadTask(pClient->recvTaskId);
			return 0;
		}
		TestData* pData = (TestData*)buf;

		//cout << "SOCKET " << pClient->s << " recv data length " << ret << endl;

		DWORD tick = timeGetTime();
		pClient->nAverage = tick - pData->sendTime;
		if (pClient->nSend % 1000 == 0)
		{
			cout << "Average time: " << pClient->nAverage*1.0 / pClient->nSend << endl;
		}
	}
	return 0;
}

Client::Client()
{
	nSend = 0;
	nAverage = 0;
	lastSendTime = timeGetTime();
}

Client::~Client()
{
}

bool Client::ConnectToServer()
{
	const char* ip = "120.79.242.138";
	uint16_t port = 12021;

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

	if (s == INVALID_SOCKET)
	{
		return false;
	}
	recvTaskId = ThreadPool::GetInstance()->AddThreadTask(RecvThread, this, MAX_TRHEAD_RUNTIMES, 1);
	sendTaskId = ThreadPool::GetInstance()->AddThreadTask(SendThread, this, MAX_TRHEAD_RUNTIMES, 1);
	isRun = true;
	return true;
}

