#pragma once
#ifndef _P1_INTERFACE
#define _P1_INTERFACE
#include <Windows.h>


__interface IIOCPTaskInterface
{
	virtual void IocpCallBack(LPVOID) = 0;
	virtual bool InitIocpTask() = 0;
};
typedef void(IIOCPTaskInterface::*CallBackFunction)(LPVOID);

struct IOCP_CONTEXT
{
	OVERLAPPED ov;
};

struct IOCP_CompletionKey
{
	CallBackFunction callback;
};

#endif // !_P1_INTERFACE

