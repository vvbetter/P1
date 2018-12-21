#pragma once
#ifndef __COMMMON_HPP__
#define __COMMMON_HPP__


#include "Interface.h"
#include <string>
#include <iostream>
#include <sstream>

#define SAFE_DELETE(x) if(x){delete (x); (x) = NULL;}
#define SAFE_DELETE_ARR(x) if(x){ delete[] (x); (x) = NULL; }

#define P1_LOG(x) \
		{std::cout<<x <<std::endl;}

inline wchar_t *multiByteToWideChar(const std::string& pKey)
{
	const char* pCStrKey = pKey.c_str();
	int pSize = MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, NULL, 0);
	wchar_t *pWCStrKey = new wchar_t[pSize];
	MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, pWCStrKey, pSize);
	return pWCStrKey;
}

inline char* wideCharToMultiByte(wchar_t* pWCStrKey)
{
	int pSize = WideCharToMultiByte(CP_OEMCP, 0, pWCStrKey, wcslen(pWCStrKey), NULL, 0, NULL, NULL);
	char* pCStrKey = new char[pSize + 1];
	WideCharToMultiByte(CP_OEMCP, 0, pWCStrKey, wcslen(pWCStrKey), pCStrKey, pSize, NULL, NULL);
	pCStrKey[pSize] = '\0';
	return pCStrKey;
}

inline static std::string GetTimeString()
{
	SYSTEMTIME st;
	GetSystemTime(&st);
	std::stringstream ss;
	ss.width(4);
	ss.fill('0');
	ss << st.wYear;
	ss.width(2);
	ss.fill('0');
	ss << st.wMonth;
	ss.width(2);
	ss.fill('0');
	ss << st.wDay;
	ss.width(2);
	ss.fill('0');
	ss << st.wHour + 8;
	ss.width(2);
	ss.fill('0');
	ss << st.wMinute;
	ss.width(2);
	ss.fill('0');
	ss << st.wSecond;
	return ss.str();
}

enum SOCKETTYPE
{
	TCP_SOCKET,
	UDP_SOCKET
};

inline HANDLE CreateSocket(SOCKETTYPE type, UINT ip, UINT16& port, bool ov = false)
{
	SOCKET s = INVALID_SOCKET;
	if (true == ov)
	{
		switch (type)
		{
		case TCP_SOCKET:
		{
			s = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		}
		case UDP_SOCKET:
		{
			s = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);
		}
		}
	}
	else
	{

	}
	if (s != INVALID_SOCKET)
	{
		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.S_un.S_addr = htonl(ip);
		int ret = bind(s, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN));
		if (ret == SOCKET_ERROR)
		{
			P1_LOG("Bind Socket Error:" << WSAGetLastError());
			closesocket(s);
			s = INVALID_SOCKET;
		}
	}
	else
	{
		P1_LOG("CreateSocket Failed");
	}
	return (HANDLE)s;
}
#endif // !__COMMMON_HPP__