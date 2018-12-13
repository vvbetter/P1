#include "stdafx.h"
#include "TraceLog.h"
#include "common.hpp"
#include <sstream>
#include <WinSock2.h>
#include <process.h>

unsigned int TraceLog::TraceLogThread(LPVOID lParam)
{
	TraceLog* pThis = (TraceLog*)lParam;
	string log = "";
	DWORD bytesWrited;
	while (pThis->_runFlag)
	{
		{
			EnterCriticalSection(&pThis->_cs);
			if (pThis->_logList.size() > 0)
			{
				log = pThis->_logList.front();
				pThis->_logList.pop_front();
			}
			LeaveCriticalSection(&pThis->_cs);
		}
		/*
			这只有一个线程，不存在线程重入的问题，否则需要考虑 ov的重入。
		*/
		if (log != "")
		{
			DWORD bytesToWrite = log.length();
			WriteFile(pThis->_fileHandle, log.c_str(), bytesToWrite, &bytesWrited, &pThis->_pLogContext->ov);
			pThis->_pLogContext->ov.Offset += bytesToWrite;
		}
		log = "";
		Sleep(1);
	}
	return 0;
}

TraceLog TraceLog::Instance()
{
	static TraceLog log;
	return log;
}

void TraceLog::IocpCallBack(LPVOID lParam)
{
	IOCP_CONTEXT* pResult = (IOCP_CONTEXT*)lParam;
	LogContext* p = CONTAINING_RECORD(pResult, LogContext, ov);
	return;
}

bool TraceLog::InitIocpTask()
{
	//创建日志文件
	_logFile = "Log_" + GetTimeString();
	wchar_t* pfilename = multiByteToWideChar(_logFile);
	_fileHandle = CreateFile(pfilename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED, NULL);
	SAFE_DELETE_ARR(pfilename);
	if (INVALID_HANDLE_VALUE == _fileHandle)
	{
		cout << "创建日志文件失败,error code:" << GetLastError() << endl;
		return false;
	}
	_pLogContext = new LogContext();
	memset(_pLogContext, 0, sizeof(LogContext));
	//初始化临界区
	InitializeCriticalSection(&_cs);
	//创建日志线程
	_runFlag = true;
	_beginthreadex(NULL, 0, TraceLogThread, this, 0, NULL);
	return true;
}

bool TraceLog::TRACELOG(stringstream is, TRACELOG_LEVEL lv)
{
	EnterCriticalSection(&_cs);
	_logList.push_back(is.str());
	LeaveCriticalSection(&_cs);
	return false;
}

TraceLog::TraceLog()
{
	_runFlag = false;
}

TraceLog::~TraceLog()
{
	CloseHandle(_fileHandle);
	SAFE_DELETE(_pLogContext);
}

