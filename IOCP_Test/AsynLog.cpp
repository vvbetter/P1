#include "pch.h"
#include "AsynLog.h"


AsynLog::AsynLog(IO_Service& io):io_service(io)
{
	logSize = 0;
	fileHandle = INVALID_HANDLE_VALUE;
	logFile = "";
}

AsynLog::~AsynLog()
{
	shutDown();
}

bool AsynLog::callBackFunction(IO_OVERLAPPED * pOv, DWORD NumberOfBytesTransferred)
{
	IO_OVERLAPPED* pIoov = CONTAINING_RECORD(pOv, IO_OVERLAPPED, ov);
	//std::cout << "#" << pIoov->ov.Offset << "#" << std::endl;
	delete pIoov;
	return true;
}

bool AsynLog::shutDown()
{
	CloseHandle(fileHandle);
	logSize = 0;
	return true;
}

bool AsynLog::initAsynLog()
{
	//创建日志文件
	logFile = "Log_" + GetTimeString() + ".txt";
	wchar_t* pfilename = multiByteToWideChar(logFile);
	fileHandle = CreateFile(pfilename, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED, NULL);
	SAFE_DELETE_ARR(pfilename);
	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		std::cout << "InitAsynLog CreateFile Error :" << GetLastError() << std::endl;
		return false;
	}
	if (!io_service.registerHandle(fileHandle, this, 8))
	{
		return false;
	}
	return false;
}

bool AsynLog::traceLog(const char* data, size_t length)
{
	if (length > 0)
	{
		DWORD bytesWrited = 0;
		LOG_OVERLAPPED* logOv = new LOG_OVERLAPPED();
		logOv->ov.Offset = static_cast<DWORD>(logSize);
		::InterlockedExchangeAdd64(&logSize, length);
		BOOL bSucess = WriteFile(fileHandle, data, length, &bytesWrited, &logOv->ov);
	}
	return true;
}
