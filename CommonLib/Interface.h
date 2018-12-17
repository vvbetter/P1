#pragma once
#ifndef _P1_INTERFACE
#define _P1_INTERFACE
#include <Windows.h>
#include <WS2tcpip.h>

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

class INetServer
{
	virtual bool InitServer() = 0;
	virtual bool UserCheck() = 0;
	virtual bool RegIOCPTask(HANDLE s) = 0;
};

struct NET_CONTEXT : public IOCP_CONTEXT
{
	WSABUF wsaBuf;
};

#endif // !_P1_INTERFACE

