#pragma once
#include "typedef.h"
#include <vector>
#include <thread>

class IO_Service
{
public:
	IO_Service();
	~IO_Service();
public:
	static void IoServiceExecuteThread(HANDLE hIoHanle, DWORD* threadState, DWORD threadId);
	//interface
	bool initService();
	bool shutDown();
	bool registerHandle(HANDLE handle, IO_COMPLETIONKEY* iock, DWORD concurrent);
	bool postIocpTask(IO_OVERLAPPED* pOv, IO_COMPLETIONKEY* iock, DWORD dwNumberOfBytesTransferred);
private:
	DWORD threadState;		//线程状态
	HANDLE ioHandle;		//完成IO Handle
	std::vector<std::thread*> vecThreads; 
};

