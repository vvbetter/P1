#include "stdafx.h"
#include "TCPServer.h"
#include "common.hpp"
#include <process.h>

bool TCPServer::InitServer()
{
	UINT16 port = 0;
	_listenSocket = (SOCKET)CreateSocket(TCP_SOCKET, 0, port, true);
	if (_listenSocket == INVALID_SOCKET)
	{
		P1_LOG("TCPServer InitServer Failed");
		return false;
	}
	_beginthreadex(NULL, 0, AcceptThread, this, 0, NULL);
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
		NET_CONTEXT* clientContext = new NET_CONTEXT();
		clientContext->wsaBuf.buf = clientContext->buf;
		clientContext->wsaBuf.len = SOCKET_BUFFER_SIZE;
		clientContext->s = (HANDLE)clientSocket;
		clientContext->pNetServer = s;
		s->_clientsContext.push_back(clientContext);
	}
	return 0;
}

TCPServer::TCPServer()
{
}


TCPServer::~TCPServer()
{
	for (auto it = _clientsContext.begin(); it != _clientsContext.end(); ++it)
	{
		NET_CONTEXT* p = (*it);
		SAFE_DELETE(p);
		it = _clientsContext.erase(it);
	}
}
