#include "stdafx.h"
#include "NetServiceTask.h"
#include "ConfigManager.h"
#include "IOCPService.h"
#include <iostream>

void NetServiceTask::IocpCallBack(LPVOID pov)
{
	NET_CONTEXT* net_context = CONTAINING_RECORD(pov, NET_CONTEXT, ov);
	if (NULL == net_context || NULL == net_context->pNetServer)
	{
		return;
	}
	net_context->pNetServer->NetServerCallBack(net_context);
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

	const ServerConfig* s_cfg = ConfigManager::GetInstance()->GetServerCfg();
	if (s_cfg == NULL)
	{
		return false;
	}
	for (auto it = s_cfg->tcpServer.begin(); it != s_cfg->tcpServer.end(); ++it)
	{
		TCPServer* _tcpServer = new TCPServer(this);
		rst = _tcpServer->InitServer(it->ip, it->uProt);
		if (rst == false)
		{
			return false;
		}
	}
	return true;
}

bool NetServiceTask::RegNewIocpTask(IOCP_CONTEXT* pContext)
{
	NET_CONTEXT* p = CONTAINING_RECORD(pContext, NET_CONTEXT, ov);
	return _io->RegisterHandle(p->s, this);
}

NetServiceTask::NetServiceTask()
{
}

NetServiceTask::~NetServiceTask()
{
	WSACleanup();
}
