// Clients.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include "Clients.h"

struct TestData
{
	USHORT length;
	USHORT mainCmd;
	USHORT subCmd;
	int data;
	TestData()
	{
		mainCmd = 0;
		subCmd = 1;
		length = sizeof(TestData);
		data = 0;
	}
};

UINT Client::SendThread(LPVOID lParam)
{
	Client* pClient = (Client*)lParam;
	static UINT n = 0;
	TestData data;
	{
		data.data = n;
		int ret = send(pClient->s, (char*)&data, sizeof(TestData), 0);
		if (ret == SOCKET_ERROR)
		{
			cout << "Socket " << pClient->s << " send Error :" << WSAGetLastError() << endl;
			ThreadPool::GetInstance()->RemoveThreadTask(pClient->sendTaskId);
			return 0;
		}
		Sleep(10);
		n++;
	}
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
		/*
		cout << "recv size: " << ret
			<< "length: " << pData->length
			<< " mainCmd: " << pData->mainCmd
			<< " subCmd: " << pData->subCmd
			<< " data: " << pData->data << endl;
		*/
	}
	return 0;
}

Client::Client()
{
	
}

Client::~Client()
{
}

bool Client::ConnectToServer()
{
	s = CreateSocketClient();
	if (s == INVALID_SOCKET)
	{
		return false;
	}
	recvTaskId = ThreadPool::GetInstance()->AddThreadTask(RecvThread, this, MAX_THREAD_RUNNING, 1);
	sendTaskId = ThreadPool::GetInstance()->AddThreadTask(SendThread, this, MAX_THREAD_RUNNING, 1);
	isRun = true;
	return true;
}

