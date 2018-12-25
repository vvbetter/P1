#include "stdafx.h"
#include "TCPServer.h"
#include "common.hpp"
#include "ThreadPool.h"
#include <process.h>

bool TCPServer::InitServer()
{
	UINT16 port = 22222;
	_listenSocket = (SOCKET)CreateSocket(TCP_SOCKET, 0, port, true);
	if (_listenSocket == INVALID_SOCKET)
	{
		P1_LOG("TCPServer InitServer Failed");
		return false;
	}
	int ret = listen(_listenSocket, SOMAXCONN);
	if (ret == SOCKET_ERROR)
	{
		P1_LOG("TCPServer Listen Error:" << WSAGetLastError());
		return false;
	}
	InitializeCriticalSection(&_cs);
	ThreadPool::GetInstance()->AddThreadTask(AcceptThread, this, MAX_TRHEAD_RUNTIMES, false);
	return true;
}

bool TCPServer::NetServerCallBack(NET_CONTEXT* pNetContext)
{
	switch (pNetContext->op)
	{
	case NET_OP_READ:
		{
			RecvNetCmdData((SOCKET)pNetContext->s, pNetContext->buf);
			RecvReq(pNetContext);
		}
		break;
	case NET_OP_WRITE:
		break;
	default:
		break;
	}

	return true;
}

bool TCPServer::SendCmdData(SOCKET s, NetCmd * pCmd)
{
	bool iResult = false;
	ClientCmd* pClientCmd = NULL;
	EnterCriticalSection(&_cs);
	auto it = _clientCmd.find((HANDLE)s);
	if (it != _clientCmd.end())
	{
		pClientCmd = it->second;
	}
	LeaveCriticalSection(&_cs);

	if (NULL != pClientCmd)
	{
		EnterCriticalSection(&pClientCmd->clientCs);
		if (NULL != pClientCmd->sendArray)
		{
			if (pClientCmd->sendArray->HasSpace())
			{
				pClientCmd->sendArray->AddItem(pCmd);
				iResult = true;
			}
		}
		LeaveCriticalSection(&pClientCmd->clientCs);
	}
	return iResult;
}

unsigned int TCPServer::AcceptThread(LPVOID lParam)
{
	//ֻ���̳߳���ע����һ���̴߳���accept��û�п�������
	TCPServer* s = (TCPServer*)lParam;
	SOCKADDR_IN addr;
	INT addrSize = sizeof(SOCKADDR_IN);

	SOCKET clientSocket = accept(s->_listenSocket, (sockaddr*)&addr, &addrSize);
	if (clientSocket == INVALID_SOCKET)
	{
		P1_LOG("Accept Socket Error:" << WSAGetLastError());
		return 0;
	}
	s->AddNewClient(clientSocket);
	return 0;
}

TCPServer::TCPServer(IIOCPTaskInterface* iotask)
{
	_ASSERT(iotask != NULL);
	_IoTaskInterface = iotask;
}


TCPServer::~TCPServer()
{
	RemoveAllClient();
	DeleteCriticalSection(&_cs);
}

bool TCPServer::RecvReq(NET_CONTEXT * clientContext)
{
	DWORD byteRecved;
	DWORD flag = 0;

	clientContext->op = NET_OP_READ;
	int iResult = WSARecv((SOCKET)clientContext->s, &clientContext->wsaRecvBuf, 1, &byteRecved, &flag, &clientContext->ov, NULL);
	if (iResult == SOCKET_ERROR)
	{
		int errCode = WSAGetLastError();
		if (errCode != WSA_IO_PENDING)
		{
			P1_LOG("TCPServer WSARecv Failed:" << WSAGetLastError());
			RemoveClient((SOCKET)clientContext->s);
			return false;
		}
	}
	return true;
}

bool TCPServer::SendReq(NET_CONTEXT * clientContext)
{
	DWORD byteSent = 0;
	DWORD flag = 0;
	clientContext->op = NET_OP_WRITE;
	int iResult = WSASend((SOCKET)clientContext->s, &clientContext->wsaSendBuf, 1, &byteSent, flag, &clientContext->ov, NULL);
	if (iResult == SOCKET_ERROR)
	{
		int errCode = WSAGetLastError();
		if (errCode != WSA_IO_PENDING)
		{
			P1_LOG("TCPServer WSARecv Failed:" << WSAGetLastError());
			RemoveClient((SOCKET)clientContext->s);
			return false;
		}
	}
	return false;
}

bool TCPServer::RemoveClient(SOCKET clientSocket)
{
	EnterCriticalSection(&_cs);
	auto it = _clientsContext.find((HANDLE)clientSocket);
	if (it != _clientsContext.end())
	{
		NET_CONTEXT* context = it->second;
		delete context;
		_clientsContext.erase(it);
	}
	auto itCmd = _clientCmd.find((HANDLE)clientSocket);
	if (itCmd != _clientCmd.end())
	{
		SOCKET s = (SOCKET)itCmd->first;
		ClientCmd* pClient = itCmd->second;
		ClearClientCmd(pClient);
		SAFE_DELETE(pClient);
		closesocket(s);
		_clientCmd.erase(itCmd);
	}
	LeaveCriticalSection(&_cs);
	return true;
}

bool TCPServer::RemoveAllClient()
{
	EnterCriticalSection(&_cs);
	for (auto it = _clientsContext.begin(); it != _clientsContext.end(); ++it)
	{
		NET_CONTEXT* p = it->second;
		SAFE_DELETE(p);
	}
	for (auto it = _clientCmd.begin(); it != _clientCmd.end(); ++it)
	{
		SOCKET s = (SOCKET)it->first;
		ClientCmd* pClient = it->second;
		ClearClientCmd(pClient);
		SAFE_DELETE(pClient);
		closesocket(s);
	}
	_clientsContext.clear();
	_clientCmd.clear();
	LeaveCriticalSection(&_cs);
	return true;
}

bool TCPServer::AddNewClient(SOCKET clientSocket)
{
	NET_CONTEXT* clientContext = new NET_CONTEXT();
	memset(clientContext->buf, 0, SOCKET_BUFFER_SIZE);
	clientContext->wsaRecvBuf.buf = clientContext->buf;
	clientContext->wsaRecvBuf.len = SOCKET_BUFFER_SIZE;
	clientContext->s = (HANDLE)clientSocket;
	clientContext->pNetServer = this;

	ClientCmd* pClientCmd = new ClientCmd();
	pClientCmd->recvArray = new SafeNetCmdArray(256);
	pClientCmd->sendArray = new SafeNetCmdArray(256);
	InitializeCriticalSection(&pClientCmd->clientCs);


	EnterCriticalSection(&_cs);
	_clientsContext[(HANDLE)clientSocket] = clientContext;
	_clientCmd[(HANDLE)clientSocket] = pClientCmd;
	LeaveCriticalSection(&_cs);

	bool iResult = _IoTaskInterface->RegNewIocpTask(clientContext);
	if (iResult)
	{
		RecvReq(clientContext);
	}
	return true;
}

bool TCPServer::ClearClientCmd(ClientCmd* pClient)
{
	if (NULL == pClient) return false;

	EnterCriticalSection(&pClient->clientCs);
	SafeNetCmdArray* pCmd = NULL;
	//ɾ�����ܶ���
	pCmd = pClient->recvArray;
	while (pCmd->HasItem())
	{
		NetCmd* p = pCmd->GetItem();
		SAFE_DELETE(p);
	}
	SAFE_DELETE(pCmd);
	//ɾ�����Ͷ���
	pCmd = pClient->sendArray;
	while (pCmd->HasItem())
	{
		NetCmd* p = pCmd->GetItem();
		SAFE_DELETE(p);
	}
	SAFE_DELETE(pCmd);
	
	LeaveCriticalSection(&pClient->clientCs);
	DeleteCriticalSection(&pClient->clientCs);
	return true;
}

bool TCPServer::RecvNetCmdData(SOCKET clientSocket, char * buf)
{
	TestNetCmd *pCmd = CreateNetCmd<TestNetCmd>(buf);
	if (NULL != pCmd)
	{
		SendCmdData(clientSocket, pCmd);
	}
	P1_LOG(buf);
	return false;
}
