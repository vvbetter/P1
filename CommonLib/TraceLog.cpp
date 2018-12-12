#include "stdafx.h"
#include "TraceLog.h"
#include "common.hpp"
#include <sstream>
#include <WinSock2.h>

struct LogContext :public IOCP_CONTEXT
{

};

TraceLog TraceLog::Instance()
{
	static TraceLog log;
	return log;
}

void TraceLog::IocpCallBack(LPVOID lParam)
{
	IOCP_CONTEXT* pResult = (IOCP_CONTEXT*)lParam;
	LogContext* p = CONTAINING_RECORD(pResult, LogContext, ov);
}

bool TraceLog::InitIocpTask()
{
	_logFile = GetTimeString();
	wchar_t* pfilename = multiByteToWideChar(_logFile);
	_fileHandle = CreateFile(pfilename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED, NULL);
	SAFE_DELETE_ARR(pfilename);
	if (INVALID_HANDLE_VALUE == _fileHandle)
	{
		cout << "创建日志文件失败,error code:" << GetLastError() << endl;
		return false;
	}
	return true;
}

TraceLog::TraceLog()
{
	
}


TraceLog::~TraceLog()
{
	CloseHandle(_fileHandle);
}

const string TraceLog::GetTimeString()
{
		SYSTEMTIME st;
		GetSystemTime(&st);
		stringstream ss;
		ss.width(4);
		ss.fill('0');
		ss << st.wYear;
		ss.width(2);
		ss.fill('0');
		ss << st.wMonth;
		ss.width(2);
		ss.fill('0');
		ss << st.wDay;
		ss.width(2);
		ss.fill('0');
		ss << st.wHour + 8;
		ss.width(2);
		ss.fill('0');
		ss << st.wMinute;
		ss.width(2);
		ss.fill('0');
		ss << st.wSecond;
		return ss.str();
}
