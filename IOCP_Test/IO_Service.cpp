#include "pch.h"
#include "IO_Service.h"
#include <iostream>
IO_Service::IO_Service()
{
	vecThreads.clear();
	threadState = 0;
	ioHandle = INVALID_HANDLE_VALUE;
}


IO_Service::~IO_Service()
{

}

void IO_Service::IoServiceExecuteThread(HANDLE hIoHanle, DWORD* threadState,DWORD threadId)
{
	DWORD NumberOfBytesTransferred;
	ULONG_PTR ulKey = 0;
	OVERLAPPED* pOv = NULL;
	while (1)
	{
		pOv = NULL;
		BOOL bSuccess = GetQueuedCompletionStatus(hIoHanle, &NumberOfBytesTransferred, &ulKey, &pOv, INFINITE);
		if (bSuccess)
		{
			if (pOv == NULL)
			{
				continue;
			}
			IO_COMPLETIONKEY* pIock = (IO_COMPLETIONKEY*)ulKey;
			IO_OVERLAPPED* pIoov = CONTAINING_RECORD(pOv, IO_OVERLAPPED, ov);
			if (pIoov->op == IO_SHUTDOWN)
			{
				(*threadState) &= ((DWORD)(-1) ^ (1 << threadId));
				std::cout << "Close Thread " << threadId << std::endl;
				break;
			}
			//处理回调
			if (pIock != NULL)
			{
				pIock->callBackFunction(pIoov, NumberOfBytesTransferred);
			}
		}
		else
		{
			std::cout << "GetQueuedCompletionStatus Failed Error: " << GetLastError() << std::endl;
		}
	}
}

bool IO_Service::initService()
{
	ioHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (ioHandle == NULL)
	{
		std::cout << "CreateIoCompletionPort Error: " << GetLastError() << std::endl;
		return false;
	}
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	for (DWORD i = 0; i < info.dwNumberOfProcessors + 2; ++i)
	{
		threadState |= (1 << i);
		std::thread* t = new std::thread(IoServiceExecuteThread, ioHandle, &threadState, i);
		vecThreads.push_back(t);
	}
	return true;
}

bool IO_Service::shutDown()
{
	DWORD dwNumberOfBytesTransferred = 0;
	IO_OVERLAPPED* pOv = new IO_OVERLAPPED();
	pOv->op = IO_SHUTDOWN;
	while(threadState)
	{
		BOOL bSucess = PostQueuedCompletionStatus(ioHandle, dwNumberOfBytesTransferred, NULL, &pOv->ov);
		if (!bSucess)
		{
			std::cout << "ShutDown PostQueuedCompletionStatus Error: " << GetLastError() << std::endl;
		}
		Sleep(10);
	}
	delete pOv;
	//清理线程
	for (auto it = vecThreads.begin(); it != vecThreads.end(); ++it)
	{
		std::thread* pThread = *it;
		pThread->join();
		delete pThread;
	}
	vecThreads.clear();
	//释放资源
	CloseHandle(ioHandle);

	return true;
}

bool IO_Service::registerHandle(HANDLE handle, IO_COMPLETIONKEY * iock, DWORD concurrent)
{
	if (ioHandle == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	HANDLE h = CreateIoCompletionPort(handle, ioHandle, (ULONG_PTR)iock, concurrent);
	if (h == NULL)
	{
		std::cout << "RegisterHandle CreateIoCompletionPort Error: " << GetLastError() << std::endl;
		return false;
	}
	return true;
}

bool IO_Service::postIocpTask(IO_OVERLAPPED * pOv, IO_COMPLETIONKEY * iock, DWORD dwNumberOfBytesTransferred)
{
	BOOL bSucess = PostQueuedCompletionStatus(ioHandle, dwNumberOfBytesTransferred, (ULONG_PTR)iock, &pOv->ov);
	if (!bSucess)
	{
		std::cout << "PostIocpTask PostQueuedCompletionStatus Error: " << GetLastError() << std::endl;
	}
	return bSucess;
}
