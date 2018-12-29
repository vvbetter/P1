#pragma once
#include "Interface.h"
#include "SafeArray.h"
#include "Cmd.h"
#include <list>
#include <map>

typedef SafeArray<NetCmd*> SafeNetCmdArray;

struct ClientCmd
{
	SafeNetCmdArray* recvArray; //接收命令队列
	SafeNetCmdArray* sendArray; //发送命令队列
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
	IIOCPTaskInterface* _IoTaskInterface;
	CRITICAL_SECTION _cs;
	SOCKET _listenSocket;
	std::map<HANDLE, NET_CONTEXT*> _clientsContext;
	std::map<HANDLE, ClientCmd* > _clientCmd;
};

