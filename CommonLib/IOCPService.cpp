#include "stdafx.h"
#include "IOCPService.h"
#include <iostream>
#include <process.h>
using namespace std;

IOCPService::IOCPService()
{
	_IocpHandle = NULL;
	_ThreadNum = 0;
}

IOCPService::~IOCPService()
{
}

unsigned  __stdcall IOCPService::CompletionPortThread(LPVOID lParam)
{
	HANDLE* IocpHandle = (HANDLE*)lParam;
	DWORD dwNoOfBytes = 0;
	ULONG_PTR ulKey = 0;
	OVERLAPPED* pov = NULL;
	while (1)
	{
		// Wait for a completion notification.  
		pov = NULL;
		BOOL fSuccess = GetQueuedCompletionStatus(
			*IocpHandle,         // Completion port handle  
			&dwNoOfBytes,  // Bytes transferred  
			&ulKey,
			&pov,          // OVERLAPPED structure  
			INFINITE       // Notification time-out interval  
		);
		if (pov == NULL)
		{
			continue;
		}
		// Get the base address of the RECEIVE_CONTEXT structure   
		// containing the OVERLAPPED structure received.  
		IOCP_CONTEXT* prc = CONTAINING_RECORD(pov, IOCP_CONTEXT, ov);
		IIOCP_CompletionKey* pKey = (IIOCP_CompletionKey*)ulKey;

	}
	return 0;
}

bool IOCPService::InitService()
{
	//创建完成io
	_IocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (NULL == _IocpHandle)
	{
		cout << "CreateIoCompletionPort error code:" << GetLastError() << endl;
		return false;
	}
	//获取cpu核心信息，创建工作线程
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	_ThreadNum = info.dwNumberOfProcessors;
	for (DWORD i = 0; i < _ThreadNum; ++i)
	{
		unsigned int threadid = 0;
		HANDLE p =(HANDLE)_beginthreadex(NULL, 0, CompletionPortThread, &_IocpHandle, 0, &threadid);
	}
	return false;
}

bool IOCPService::RegisterHandle(HANDLE handle, LPVOID CompletionKey)
{
	if (NULL == _IocpHandle)
	{
		return false;
	}
	HANDLE rHandle = CreateIoCompletionPort(handle, _IocpHandle, (ULONG_PTR)CompletionKey, _ThreadNum);
	if (NULL == rHandle)
	{
		cout << "CreateIoCompletionPort associated with handle error code: " << GetLastError() << endl;
		return false;
	}
	return true;
}
