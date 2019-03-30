#pragma once
#include "typedef.h"
#include "IO_Service.h"
#include <string>

struct LOG_OVERLAPPED :public IO_OVERLAPPED
{
	LOG_OVERLAPPED()
	{
		op = IO_WRITE;
		memset(&this->ov, 0, sizeof(this->ov));
	}
};

class AsynLog :public IO_COMPLETIONKEY
{
public:
	AsynLog(IO_Service& io);
	~AsynLog();

public:
	virtual bool callBackFunction(IO_OVERLAPPED* pOv, DWORD NumberOfBytesTransferred);
	virtual bool shutDown();
	bool initAsynLog();
	bool traceLog(const char* data, size_t length);
private:
	IO_Service& io_service;
	std::string logFile;
	HANDLE fileHandle;
	int64_t logSize;
};

