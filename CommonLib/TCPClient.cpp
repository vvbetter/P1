#include "stdafx.h"
#include "TCPClient.h"
#include "common.hpp"
#include "ThreadPool.h"

bool TCPClient::InitServer(const std::string & ip, const USHORT port)
{
	uint16_t bindport = 0;
	_clientSocket = (SOCKET)CreateSocket(TCP_SOCKET, ip, bindport, true);
	if (_clientSocket == INVALID_SOCKET)
	{
		return false;
	}
	sockaddr_in sAddr;
	sAddr.sin_family = AF_INET;
	sAddr.sin_port = htons(port);
	sAddr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
	int iResult = ::connect(_clientSocket, (sockaddr*)&sAddr, sizeof(sockaddr_in));
	if (SOCKET_ERROR == iResult)
	{
		P1_LOG("TCPClient InitServer Failed: " << WSAGetLastError());
		closesocket(_clientSocket);
		return false;
	}
	ThreadPool::GetInstance()->AddThreadTask(ClientSendThread, this, MAX_TRHEAD_RUNTIMES, 1);

	_pNetContext = new NET_CONTEXT();
	memset(_pNetContext, 0, sizeof(NET_CONTEXT));
	_pNetContext->pNetServer = this;
	_pNetContext->wsaRecvBuf.buf = _pNetContext->buf;
	_pNetContext->wsaRecvBuf.len = SOCKET_BUFFER_SIZE;
	_pNetContext->s = (HANDLE)_clientSocket;
	_pNetContext->op = NET_OP_READ;
	bool bResult = _iotask->RegNewIocpTask(_pNetContext);

	return bResult;
}

bool TCPClient::NetServerCallBack(NET_CONTEXT *)
{
	return false;
}

bool TCPClient::SendCmdData(SOCKET s, NetCmd * pCmd)
{
	assert(pCmd != NULL);
	if (NULL == pCmd)
	{
		return false;
	}
	return _sendQueue.bounded_push(pCmd);
}

UINT TCPClient::ClientSendThread(LPVOID lParam)
{
	TCPClient* pClinet = (TCPClient*)lParam;
	return 0;
}

TCPClient::TCPClient(IIOCPTaskInterface* iotask):_sendQueue(200),_recvQueue(200)
{
	_iotask = iotask;
}


TCPClient::~TCPClient()
{
}
