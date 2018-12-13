#pragma once
#include "Interface.h"

class IOCPService
{
public:
	IOCPService();
	~IOCPService();
	static unsigned WINAPI CompletionPortThread(LPVOID lParam);
public:
	bool InitService();
	bool RegisterHandle(HANDLE handle, IIOCPTaskInterface* CompletionKey = NULL);
	const HANDLE IOCPServiceHandle() { return _IocpHandle; }
private:
	HANDLE _IocpHandle;
	DWORD _ThreadNum;
};

