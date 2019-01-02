#include "stdafx.h"
#include "TCPServer.h"
#include "common.hpp"
#include "ThreadPool.h"
#include "TimerManager.h"
#include <process.h>


constexpr UINT FRAME_NUMBER_SEND_DATA = 100; //每个玩家每帧发送命令数量

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
	ThreadPool::GetInstance()->AddThreadTask(AcceptThread, this, MAX_TRHEAD_RUNTIMES, 1);
	ThreadPool::GetInstance()->AddThreadTask(SendThread, this, MAX_TRHEAD_RUNTIMES, 4);
	TimerManager::GetInstance()->CreateTimer(HeartCheckTimer, this, 5000);
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
	_ASSERT(pCmd->length < SOCKET_BUFFER_SIZE);
	if (pCmd->length >= SOCKET_BUFFER_SIZE)
	{
		return false;
	}
	bool iResult = false;
	ClientCmd* pClientCmd = NULL;
	{
		boost::shared_lock_guard<boost::shared_mutex> lk(_mutex);
		auto it = _clientCmd.find((HANDLE)s);
		if (it != _clientCmd.end())
		{
			pClientCmd = it->second;
			if (pClientCmd)
			{
				if (!pClientCmd->sendQueue.bounded_push(pCmd))
				{
					P1_LOG("Client " << s << " does not have space!");
				}
			}
		}
	}
	return iResult;
}

unsigned int TCPServer::AcceptThread(LPVOID lParam)
{
	//只在线程池中注册了一个线程处理accept，没有考虑重入
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

unsigned int TCPServer::SendThread(LPVOID lParam)
{
	TCPServer* pServer = (TCPServer*)lParam;
	std::map<SOCKET, std::vector< NetCmd*> >tempCmds;
	auto& clientCmds = pServer->_clientCmd;
	{
		boost::shared_lock_guard<boost::shared_mutex> lk(pServer->_mutex);
		for (auto it = clientCmds.begin(); it != clientCmds.end(); ++it)
		{
			UINT cmdLength = 0;
			UINT cmdCount = 0;
			std::vector<NetCmd*> vecCmds(FRAME_NUMBER_SEND_DATA, NULL);
			SOCKET clientSocket = (SOCKET)it->first;
			SafeNetCmdQueue& sendQueue = it->second->sendQueue;
			while (cmdCount < FRAME_NUMBER_SEND_DATA)
			{
				NetCmd* pCmd = NULL;
				if (!sendQueue.pop<NetCmd*>(pCmd))
				{
					break;
				}
				if (pCmd == NULL)
				{
					continue;
				}
				if (cmdLength + pCmd->length < SOCKET_BUFFER_SIZE)
				{
					vecCmds[cmdCount] = pCmd;
					++cmdCount;
					cmdLength += pCmd->length;
				}
				else
				{
					//缓冲区满，放回队列，等待下一次发送
					sendQueue.bounded_push(pCmd);
					break;
				}
			}
			tempCmds[clientSocket] = vecCmds;
		}
	}
	if (tempCmds.size() == 0)
	{
		return 0;
	}
	//定义发送缓冲区
	char* sendBuff = new char[SOCKET_BUFFER_SIZE];
	for (auto it = tempCmds.begin(); it != tempCmds.end(); ++it)
	{
		SOCKET clientSocket = it->first;
		auto& vecCmds = it->second;
		//组包
		UINT offset = 0;
		for (int i = 0; i < FRAME_NUMBER_SEND_DATA; )
		{
			NetCmd* pCmd = vecCmds[i];
			if (NULL != pCmd)
			{
				if (offset + pCmd->length < SOCKET_BUFFER_SIZE)
				{
					memcpy_s(sendBuff + offset, pCmd->length, pCmd, pCmd->length);
					offset += pCmd->length;
					SAFE_DELETE(pCmd);
				}
				else
				{
					pServer->SendReq(clientSocket, sendBuff, offset);
					offset = 0;
					continue;
				}
			}
			++i;
		}
		if (offset != 0)
		{
			pServer->SendReq(clientSocket, sendBuff, offset);
		}
	}
	delete[] sendBuff;
	return 0;
}

void TCPServer::HeartCheckTimer(LPVOID lParam)
{
	TCPServer * pServer = (TCPServer*)lParam;
	std::vector<SOCKET> invalidSocket;
	{
		boost::shared_lock_guard<boost::shared_mutex> lk(pServer->_mutex);
		for (auto it = pServer->_clientsContext.begin(); it != pServer->_clientsContext.end(); ++it)
		{
			if (!pServer->CheckSocketAvailable((SOCKET)it->first))
			{
				invalidSocket.push_back((SOCKET)it->first);
			}
		}
	}
	for (auto it = invalidSocket.begin(); it != invalidSocket.end(); ++it)
	{
		pServer->RemoveClient(*it);
		P1_LOG("Remove Socket :" << *it);
	}
}

TCPServer::TCPServer(IIOCPTaskInterface* iotask)
{
	_ASSERT(iotask != NULL);
	_IoTaskInterface = iotask;
}


TCPServer::~TCPServer()
{
	RemoveAllClient();
}

bool TCPServer::RecvReq(NET_CONTEXT * clientContext)
{
	DWORD byteRecved;
	DWORD flag = 0;
	memset(clientContext->buf, 0, SOCKET_BUFFER_SIZE);
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

bool TCPServer::SendReq(SOCKET s, char* buff, UINT length)
{
	bool ret = false;
	int iResult = SOCKET_ERROR;
	DWORD byteSent = 0;
	DWORD flag = 0;
	{
		boost::shared_lock_guard<boost::shared_mutex> lk(_mutex);
		auto it = _clientsContext.find((HANDLE)s);
		if (it != _clientsContext.end())
		{
			NET_CONTEXT* pContext = it->second;
			pContext->wsaSendBuf.buf = buff;
			pContext->wsaSendBuf.len = length;
			iResult = WSASend(s, &pContext->wsaSendBuf, 1, &byteSent, flag, &pContext->ov, NULL);
			DWORD tick = timeGetTime();
			P1_LOG("SOCKET " << s << " Time = " << tick << " Senddata length =" << length << " data= " << ((TestNetCmd*)buff)->data << " use time " << (tick - ((TestNetCmd*)buff)->recvTime));
			ret = true;
		}
	}

	if (ret == false)
	{
		return false;
	}
	if (iResult == SOCKET_ERROR)
	{
		int errCode = WSAGetLastError();
		if (errCode != WSA_IO_PENDING)
		{
			P1_LOG("TCPServer WSARecv Failed:" << WSAGetLastError());
			RemoveClient(s);
			return false;
		}
	}
	return false;
}

bool TCPServer::RemoveClient(SOCKET clientSocket)
{
	boost::unique_lock<boost::shared_mutex> lk(_mutex);
	auto it = _clientsContext.find((HANDLE)clientSocket);
	if (it != _clientsContext.end())
	{
		NET_CONTEXT* context = it->second;
		context->pNetServer = NULL;
		SAFE_DELETE(context);
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
	return true;
}

bool TCPServer::RemoveAllClient()
{
	boost::unique_lock<boost::shared_mutex> lk(_mutex);
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

	ClientCmd* pClientCmd = new ClientCmd(1024);
	{
		boost::unique_lock<boost::shared_mutex> lk(_mutex);
		_clientsContext[(HANDLE)clientSocket] = clientContext;
		_clientCmd[(HANDLE)clientSocket] = pClientCmd;
	}
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
	NetCmd* pCmd = NULL;
	//删除接受队列
	SafeNetCmdQueue& recvQueue = pClient->recvQueue;
	while (recvQueue.pop<NetCmd*>(pCmd))
	{
		if (pCmd)
		{
			SAFE_DELETE(pCmd);
		}
	}
	//删除发送队列
	SafeNetCmdQueue& sendQueue = pClient->sendQueue;
	while (sendQueue.pop<NetCmd*>(pCmd))
	{
		if (pCmd)
		{
			SAFE_DELETE(pCmd);
		}
	}
	return true;
}

bool TCPServer::RecvNetCmdData(SOCKET clientSocket, char * buf)
{
	UINT offset = 0;
	DWORD tick = timeGetTime();
	while (offset < SOCKET_BUFFER_SIZE)
	{
		NetCmd* pCmd = (NetCmd*)(buf + offset);
		USHORT len = pCmd->length;
		if (0 == len)
		{
			break;
		}
		offset += len;
		USHORT mainCmd = pCmd->mainCmd;
		USHORT subCmd = pCmd->subCmd;
		switch (mainCmd)
		{
		case CMD_TEST:
		{
			TestNetCmd *pCmd = CreateNetCmd<TestNetCmd>(buf);
			if (NULL != pCmd)
			{
				pCmd->recvTime = tick;
				SendCmdData(clientSocket, pCmd);
			}
			else
			{
				P1_LOG("CreateNetCmd Error MainCmd:" << mainCmd << " SubCmd: :" << subCmd << " length :" << len);
			}
		}
		default:
			break;
		}
	}
	return false;
}

bool TCPServer::CheckSocketAvailable(SOCKET s)
{
	int iCheck = send(s, "", 0, 0);
	if (iCheck == SOCKET_ERROR)
	{
		return false;
	}
	return true;
}
