#pragma once
#include "Interface.h"
class UDPServer : public INetServer
{
public:
	static unsigned int __stdcall UserCheckThread(LPVOID);

	virtual bool InitServer();
	virtual bool UserCheck();
	virtual bool RegIOCPTask(HANDLE s);
public:
	UDPServer();
	virtual ~UDPServer();
private:
	SOCKET _server;
};

