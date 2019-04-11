#include "pch.h"
#include "AsynLog.h"

AsynLog* g_LogHandler;
void Log(const CHAR *pcStr, ...)
{
	va_list var;
	va_start(var, pcStr);
	UINT nCount = _vscprintf(pcStr, var);
	std::string timeStr = GetTimeStr();
	size_t bufSize = nCount + timeStr.length() + 1;
	CHAR *pBuffer = new CHAR[bufSize];
	memset(pBuffer, 0, bufSize);
	if (pBuffer)
	{
		sprintf_s(pBuffer, bufSize, "%s", timeStr.c_str());
		vsprintf_s(pBuffer + timeStr.length(), nCount + 1, pcStr, var);
		if (g_LogHandler == NULL)
		{
			std::cout << pBuffer;
		}
		else
		{
			g_LogHandler->traceLog(pBuffer);
		}
		delete[] pBuffer;
	}
	va_end(var);
}

AsynLog::AsynLog(IO_Service& io):io_service(io)
{
	logSize = 0;
	fileHandle = INVALID_HANDLE_VALUE;
	logFile = "";
	g_LogHandler = this;
}

AsynLog::~AsynLog()
{
	g_LogHandler = NULL;
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
	if (!io_service.registerHandle(fileHandle, this, io_service.threadSums()))
	{
		return false;
	}
	return true;
}

bool AsynLog::traceLog(const char* data)
{
	size_t length = strlen(data);
	if (length > 0)
	{
		DWORD bytesWrited = 0;
		LOG_OVERLAPPED* logOv = new LOG_OVERLAPPED();
		logOv->ov.Offset = static_cast<DWORD>(logSize);
		::InterlockedExchangeAdd64(&logSize, length);
		BOOL bSucess = WriteFile(fileHandle, data, length, &bytesWrited, &logOv->ov);
		if (!bSucess && GetLastError() != ERROR_IO_PENDING)
		{
			Log("Log Data Error: ec = %d, data = %s", GetLastError(), data);
			return false;
		}
	}
	return true;
}
