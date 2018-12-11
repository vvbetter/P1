#pragma once
#ifndef _P1_INTERFACE
#define _P1_INTERFACE

#include <Windows.h>


struct IOCP_CONTEXT
{
	OVERLAPPED ov;
};

struct IIOCP_CompletionKey
{
	LPVOID callback;
};

__interface INetWorkInterface
{

};

#endif // !_P1_INTERFACE

