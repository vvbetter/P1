#include "stdafx.h"
#include "TCPServer.h"
#include "common.hpp"
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
	_beginthreadex(NULL, 0, AcceptThread, this, 0, NULL);
	return true;
}

bool TCPServer::NetServerCallBack(NET_CONTEXT* pNetContext)
{
	P1_LOG(pNetContext->buf);
	RecvReq(pNetContext);
	return true;
}

unsigned int __stdcall TCPServer::AcceptThread(LPVOID lParam)
{
	TCPServer* s = (TCPServer*)lParam;
	SOCKADDR_IN addr;
	INT addrSize = sizeof(SOCKADDR_IN);
	while (1)
	{
		SOCKET clientSocket = accept(s->_listenSocket, (sockaddr*)&addr, &addrSize);
		if (clientSocket == INVALID_SOCKET)
		{
			P1_LOG("Accept Socket Error:" << WSAGetLastError());
			continue;
		}
		s->AddNewClient(clientSocket);
	}
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
	int iResult = WSARecv((SOCKET)clientContext->s, &clientContext->wsaBuf, SOCKET_BUFFER_SIZE, &byteRecved, 0, &clientContext->ov, NULL);
	if (iResult != WSA_IO_PENDING)
	{
		P1_LOG("TCPServer WSARecv Failed" << WSAGetLastError());
		return false;
	}
	return true;
}

bool TCPServer::RemoveClient(SOCKET clientSocket)
{
	EnterCriticalSection(&_cs);
	auto it = _clientsContext.find((HANDLE)clientSocket);
	if (it != _clientsContext.end())
	{
		NET_CONTEXT* context = it->second;
		delete context;
	}
	auto itCmd = _clientCmd.find((HANDLE)clientSocket);
	if (itCmd != _clientCmd.end())
	{
		SOCKET s = (SOCKET)itCmd->first;
		SafeNetCmdArray* pCmd = itCmd->second;
		while (pCmd->HasItem())
		{
			NetCmd* p = pCmd->GetItem();
			SAFE_DELETE(p);
		}
		closesocket(s);
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
		it = _clientsContext.erase(it);
	}
	for (auto it = _clientCmd.begin(); it != _clientCmd.end(); ++it)
	{
		SOCKET s = (SOCKET)it->first;
		SafeNetCmdArray* pCmd = it->second;
		while (pCmd->HasItem())
		{
			NetCmd* p = pCmd->GetItem();
			SAFE_DELETE(p);
		}
		closesocket(s);
	}
	LeaveCriticalSection(&_cs);
	return true;
}

bool TCPServer::AddNewClient(SOCKET clientSocket)
{
	NET_CONTEXT* clientContext = new NET_CONTEXT();
	memset(clientContext->buf, 0, SOCKET_BUFFER_SIZE);
	clientContext->wsaBuf.buf = clientContext->buf;
	clientContext->wsaBuf.len = SOCKET_BUFFER_SIZE;
	clientContext->s = (HANDLE)clientSocket;
	clientContext->pNetServer = this;
	SafeNetCmdArray* cmdArray = new SafeNetCmdArray(256);

	EnterCriticalSection(&_cs);
	_clientsContext[(HANDLE)clientSocket] = clientContext;
	_clientCmd[(HANDLE)clientSocket] = cmdArray;
	LeaveCriticalSection(&_cs);

	_IoTaskInterface->RegNewIocpTask(clientContext);
	
	RecvReq(clientContext);
	return true;
}
