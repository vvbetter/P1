#pragma once
#include "typedef.h"
#include "IO_Service.h"

struct UDPSERVER_OVERLAPPED :public IO_OVERLAPPED
{
	UDPSERVER_OVERLAPPED()
	{
		addrLen = sizeof(sendAddr);
		bytesRecved = 0;
		wsaBuf.buf = data;
		wsaBuf.len = 50;
		memset(data, 0, 50);
	}
	char data[50];
	WSABUF wsaBuf;
	DWORD bytesRecved;
	sockaddr_in	sendAddr;
	INT addrLen;
};

class UdpServer :public IO_COMPLETIONKEY
{
public:
	UdpServer(IO_Service& io);
	~UdpServer();
public:
	virtual bool callBackFunction(IO_OVERLAPPED* pOv, DWORD NumberOfBytesTransferred);
	virtual bool shutDown();

	bool initServer(USHORT port);
private:
	IO_Service& ioService;
	SOCKET recvSocekt;
	USHORT port;

	volatile unsigned int n;
	UDPSERVER_OVERLAPPED * sOV;
};

