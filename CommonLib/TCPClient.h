#pragma once
#include "Interface.h"
#include <boost/lockfree/queue.hpp>

class TCPClient :public INetService
{
public:
	//interface
	virtual bool InitServer(const std::string& ip, const USHORT port);
	virtual bool NetServerCallBack(NET_CONTEXT*);
	virtual bool SendCmdData(SOCKET s, NetCmd* pCmd);
	//工作线程
	static UINT ClientSendThread(LPVOID);
public:

public:
	TCPClient(IIOCPTaskInterface* iotask);
	virtual ~TCPClient();
private:
	SOCKET _clientSocket;
	boost::lockfree::queue<NetCmd*> _sendQueue;
	boost::lockfree::queue<NetCmd*> _recvQueue;
	IIOCPTaskInterface* _iotask;
	NET_CONTEXT* _pNetContext;
};

