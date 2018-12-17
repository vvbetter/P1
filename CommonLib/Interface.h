#pragma once
#ifndef _P1_INTERFACE
#define _P1_INTERFACE
#include <Windows.h>

class IOCPService;

class IIOCPTaskInterface
{
public:
	virtual void IocpCallBack(LPVOID) = 0;
	virtual bool InitIocpTask(IOCPService* io_service) = 0;
};

struct IOCP_CONTEXT
{
	OVERLAPPED ov;
};
typedef void(IIOCPTaskInterface::*CallBackFunction)(LPVOID);

#endif // !_P1_INTERFACE

