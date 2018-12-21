#pragma once
#include "Interface.h"
#include "TCPServer.h"

class NetServiceTask : public IIOCPTaskInterface
{
public:
	virtual void IocpCallBack(LPVOID);
	virtual bool InitIocpTask(IOCPService* io_service);
	virtual bool RegNewIocpTask(IOCP_CONTEXT*);
public:
	NetServiceTask();
	virtual ~NetServiceTask();
private:
	IOCPService* _io;
	TCPServer* _tcpServer;
};

