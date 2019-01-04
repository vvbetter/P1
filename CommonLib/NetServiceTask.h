#pragma once
#include "Interface.h"
#include "TCPServer.h"
#include <map>

class NetServiceTask : public IIOCPTaskInterface
{
public:
	virtual void IocpCallBack(LPVOID);
	virtual bool InitIocpTask(IOCPService* io_service);
	virtual bool RegNewIocpTask(IOCP_CONTEXT*);
public:
	TCPServer* QueryTcpServer(const std::string& ip, const uint16_t port);
public:
	NetServiceTask();
	virtual ~NetServiceTask();
private:
	IOCPService* _io;
	std::map<TCPServer*, std::string> _tcpServers;
};

