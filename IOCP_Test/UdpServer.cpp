#include "pch.h"
#include "UdpServer.h"


UdpServer::UdpServer(IO_Service& io):ioService(io)
{
	recvSocekt = INVALID_SOCKET;
	n = 0;
}


UdpServer::~UdpServer()
{
}

bool UdpServer::callBackFunction(IO_OVERLAPPED * lpOV, DWORD NumberOfBytesTransferred)
{
	UDPSERVER_OVERLAPPED* pOv = CONTAINING_RECORD(lpOV, UDPSERVER_OVERLAPPED, ov);
	::InterlockedIncrement(&n);
	if (n % 100000 == 0)
	{
		Log("this is n = %u  bytes = %d\n", n, pOv->bytesRecved);
	}
	DWORD flag = 0;
	WSARecvFrom(recvSocekt, &(pOv->wsaBuf), 1, &(pOv->bytesRecved), &flag, (SOCKADDR*)&(pOv->sendAddr), &(pOv->addrLen), &(pOv->ov), NULL);
	return true;
}

bool UdpServer::shutDown()
{
	return false;
}

bool UdpServer::initServer(USHORT port)
{
	this->port = port;
	int iResult = 0;
	WSADATA wsaData;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		Log("UdpServer WSAStartup Failed With Error %d\n", iResult);
		return false;
	}
	recvSocekt = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	int recvBufSize = 500 * 1000000;
	setsockopt(recvSocekt, SOL_SOCKET, SO_RCVBUF, (char*)&recvBufSize, sizeof(int));
	if (recvSocekt == INVALID_SOCKET)
	{
		Log("UdpServer Create RecvSocket Error %d\n", WSAGetLastError());
		return false;
	}
	sockaddr_in RecvAddr;
	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_port = htons(port);
	RecvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	iResult = ::bind(recvSocekt, (SOCKADDR*)&RecvAddr, sizeof(RecvAddr));
	if (iResult != 0) {
		Log("UdpServer Bind Failed With Error %d", WSAGetLastError());
		return false;
	}
	if (!ioService.registerHandle((HANDLE)recvSocekt, this, 0))
	{
		return false;
	}
	DWORD flag = 0;
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	for (DWORD i = 0; i < info.dwNumberOfProcessors; ++i)
	{
		UDPSERVER_OVERLAPPED * pOv = new UDPSERVER_OVERLAPPED();
		WSARecvFrom(recvSocekt, &(pOv->wsaBuf), 1, &(pOv->bytesRecved), &flag, (SOCKADDR*)(&(pOv->sendAddr)), &(pOv->addrLen), &(pOv->ov), NULL);
	}
	return true;
}
