#pragma once
#include "Interface.h"
#include <list>
class TCPServer : public INetServer
{
public:
	virtual bool InitServer();
	static unsigned int __stdcall AcceptThread(LPVOID);
public:
	TCPServer();
	virtual ~TCPServer();
private:
	SOCKET _listenSocket;
	std::list<NET_CONTEXT*> _clientsContext;
};

