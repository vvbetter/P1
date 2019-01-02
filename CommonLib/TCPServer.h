#pragma once
#include "Interface.h"
#include "SafeArray.h"
#include "Cmd.h"
#include <map>
#include <boost/lockfree/queue.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>

typedef boost::lockfree::queue<NetCmd*> SafeNetCmdQueue;

struct ClientCmd
{
	SafeNetCmdQueue recvQueue; //接收命令队列
	SafeNetCmdQueue sendQueue; //发送命令队列
	ClientCmd(UINT QueueSize) :recvQueue(QueueSize), sendQueue(QueueSize) {}
};

class TCPServer : public INetServer
{
public:
	//interface
	virtual bool InitServer();
	virtual bool NetServerCallBack(NET_CONTEXT*);
	virtual bool SendCmdData(SOCKET s, NetCmd* pCmd);
	//工作线程
	static unsigned int  AcceptThread(LPVOID);
	static unsigned int SendThread(LPVOID);
	static void HeartCheckTimer(LPVOID);
public:
	TCPServer(IIOCPTaskInterface* iotask);
	virtual ~TCPServer();

public:
	bool RecvReq(NET_CONTEXT* clientContext);
	bool SendReq(SOCKET s, char* buff, UINT length);
	bool RemoveClient(SOCKET clientSocket);
private:
	bool RemoveAllClient();
	bool AddNewClient(SOCKET clientSocket);
	bool ClearClientCmd(ClientCmd* pClient);
	bool RecvNetCmdData(SOCKET clientSocket, char* buf);
	bool CheckSocketAvailable(SOCKET s);
private:
	boost::shared_mutex _mutex;
	IIOCPTaskInterface* _IoTaskInterface;
	SOCKET _listenSocket;
	std::map<HANDLE, NET_CONTEXT*> _clientsContext;
	std::map<HANDLE, ClientCmd* > _clientCmd;
};

