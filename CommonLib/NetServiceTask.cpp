#include "stdafx.h"
#include "NetServiceTask.h"
#include "IOCPService.h"
#include <iostream>
#include <WinSock2.h>

void NetServiceTask::IocpCallBack(LPVOID)
{
}

bool NetServiceTask::InitIocpTask(IOCPService * io_service)
{
	WSADATA wsaData;
	bool rst = false;
	rst = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (rst != 0) {
		std::cout << "WSAStartup failed: " << WSAGetLastError() << std::endl;
		return false;
	}
	_io = io_service;
	return true;
}

NetServiceTask::NetServiceTask()
{
}

NetServiceTask::~NetServiceTask()
{
	WSACleanup();
}
