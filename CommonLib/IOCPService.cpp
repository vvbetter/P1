#include "stdafx.h"
#include "IOCPService.h"
#include <iostream>
#include <process.h>
using namespace std;

constexpr UINT IOCP_TIME_OUT = 5000;//Milliseconds

IOCPService::IOCPService()
{
	_IocpHandle = NULL;
	_ThreadNum = 0;
}

IOCPService::~IOCPService()
{
	CloseHandle(_IocpHandle);
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
			&dwNoOfBytes,		// Bytes transferred  
			&ulKey,
			&pov,				// OVERLAPPED structure  
			IOCP_TIME_OUT       // Notification time-out interval  
		);
		if (!fSuccess)
		{
			DWORD eCode = GetLastError();
			switch (eCode)
			{
			case WAIT_TIMEOUT:
			default:
				continue;
			}
		}

		if (NULL == pov)
		{
			continue;
		}
		// Get the base address of the RECEIVE_CONTEXT structure   
		// containing the OVERLAPPED structure received.  
		IIOCPTaskInterface* pBase = (IIOCPTaskInterface*)ulKey;
		if (NULL == pBase) continue;
		(pBase->IocpCallBack)(pov);
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
	return true;
}

bool IOCPService::RegisterHandle(HANDLE handle, IIOCPTaskInterface* CompletionKey)
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
