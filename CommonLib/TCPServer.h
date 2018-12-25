#pragma once
#include "Interface.h"
#include "SafeArray.h"
#include "Cmd.h"
#include <list>
#include <map>

typedef SafeArray<NetCmd*> SafeNetCmdArray;

struct ClientCmd
{
	SafeNetCmdArray* recvArray; //�����������
	SafeNetCmdArray* sendArray; //�����������
	CRITICAL_SECTION clientCs;  //ÿ����ҵ�������
};

class TCPServer : public INetServer
{
public:
	virtual bool InitServer();
	virtual bool NetServerCallBack(NET_CONTEXT*);
	virtual bool SendCmdData(SOCKET s, NetCmd* pCmd);

	static unsigned int  AcceptThread(LPVOID);
public:
	TCPServer(IIOCPTaskInterface* iotask);
	virtual ~TCPServer();

public:
	bool RecvReq(NET_CONTEXT* clientContext);
	bool SendReq(NET_CONTEXT* clientContext);
	bool RemoveClient(SOCKET clientSocket);
private:
	bool RemoveAllClient();
	bool AddNewClient(SOCKET clientSocket);
	bool ClearClientCmd(ClientCmd* pClient);
	bool RecvNetCmdData(SOCKET clientSocket, char* buf);
private:
	IIOCPTaskInterface* _IoTaskInterface;
	CRITICAL_SECTION _cs;
	SOCKET _listenSocket;
	std::map<HANDLE, NET_CONTEXT*> _clientsContext;
	std::map<HANDLE, ClientCmd* > _clientCmd;
};

