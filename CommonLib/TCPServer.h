#pragma once
#include "Interface.h"
#include "SafeArray.h"
#include <list>
#include <map>

typedef SafeArray<NetCmd*> SafeNetCmdArray;

class TCPServer : public INetServer
{
public:
	virtual bool InitServer();
	virtual bool NetServerCallBack(NET_CONTEXT*);
	static unsigned int __stdcall AcceptThread(LPVOID);
public:
	TCPServer(IIOCPTaskInterface* iotask);
	virtual ~TCPServer();

public:
	bool RecvReq(NET_CONTEXT* clientContext);
	bool RemoveClient(SOCKET clientSocket);
private:
	bool RemoveAllClient();
	bool AddNewClient(SOCKET clientSocket);
private:
	IIOCPTaskInterface* _IoTaskInterface;
	CRITICAL_SECTION _cs;
	SOCKET _listenSocket;
	std::map<HANDLE, NET_CONTEXT*> _clientsContext;
	std::map<HANDLE, SafeNetCmdArray* > _clientCmd;
};

