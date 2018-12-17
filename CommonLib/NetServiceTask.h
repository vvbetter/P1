#pragma once
#include "Interface.h"
#include "Interface.h"


class NetServiceTask : public IIOCPTaskInterface
{
public:
	virtual void IocpCallBack(LPVOID);
	virtual bool InitIocpTask(IOCPService* io_service);
public:
	NetServiceTask();
	virtual ~NetServiceTask();
private:
	IOCPService* _io;
};

