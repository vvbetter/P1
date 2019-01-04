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
	//初始化服务器
	for (auto it = s_cfg->tcpServer.begin(); it != s_cfg->tcpServer.end(); ++it)
	{
		TCPServer* tcpServer = new TCPServer(this);
		rst = tcpServer->InitServer("0", it->uProt);
		if (rst == false)
		{
			delete tcpServer;
			return false;
		}
		std::stringstream ss;
		ss << it->ip << ":" << it->uProt;
		_tcpServers.insert(std::make_pair(tcpServer, ss.str()));
	}
	return true;
}

bool NetServiceTask::RegNewIocpTask(IOCP_CONTEXT* pContext)
{
	NET_CONTEXT* p = CONTAINING_RECORD(pContext, NET_CONTEXT, ov);
	return _io->RegisterHandle(p->s, this);
}

TCPServer * NetServiceTask::QueryTcpServer(const std::string & ip, const uint16_t port)
{
	std::stringstream ss;
	ss << ip << ":" << port;
	for(auto it = _tcpServers.begin(); it != _tcpServers.end(); ++it)
	{
		if (it->second == ss.str())
		{
			return it->first;
		}
	}
	return NULL;
}

NetServiceTask::NetServiceTask()
{
}

NetServiceTask::~NetServiceTask()
{
	WSACleanup();
}
