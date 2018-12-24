#pragma once
#ifndef _P1_INTERFACE
#define _P1_INTERFACE
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <timeapi.h>

constexpr UINT32 SOCKET_BUFFER_SIZE = 1024*1024;

class IOCPService; 
class INetServer;

enum NET_OPERATOR
{
	NET_OP_READ = 1,
	NET_OP_WRITE,
};
struct IOCP_CONTEXT
{
	OVERLAPPED ov;
};
struct NET_CONTEXT : public IOCP_CONTEXT
{
	WSABUF wsaBuf;
	char buf[SOCKET_BUFFER_SIZE];
	HANDLE s;
	INetServer* pNetServer;
	NET_OPERATOR op;
};
class IIOCPTaskInterface
{
public:
	virtual void IocpCallBack(LPVOID) = 0;
	virtual bool InitIocpTask(IOCPService* io_service) = 0;
	virtual bool RegNewIocpTask(IOCP_CONTEXT*) = 0;
};

typedef void(IIOCPTaskInterface::*CallBackFunction)(LPVOID);

class INetServer
{
public:
	virtual bool InitServer() = 0;
	virtual bool NetServerCallBack(NET_CONTEXT*) = 0;
};
//ÍøÂçÃüÁîÊý¾ÝÍ·
struct NetCmd
{
	BYTE mainCmd;
	BYTE subCmd;
	USHORT length;
};

#endif // !_P1_INTERFACE

