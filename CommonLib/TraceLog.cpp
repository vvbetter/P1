#include "stdafx.h"
#include "TraceLog.h"
#include "common.hpp"
#include "IOCPService.h"
#include "ThreadPool.h"
#include <sstream>
#include <WinSock2.h>
#include <process.h>

TraceLog * TraceLog::GetInstance()
{
	static TraceLog log;
	return &log;
}

UINT TraceLog::TraceLogThread(LPVOID lParam)
{
	TraceLog* pThis = (TraceLog*)lParam;
	string log = "";
	DWORD bytesWrited;
	{
		EnterCriticalSection(&pThis->_cs);
		while (pThis->_logList.size() > 0)
		{
			log += pThis->_logList.front();
			pThis->_logList.pop_front();
		}
		LeaveCriticalSection(&pThis->_cs);
	}
	/*
		��ֻ��һ���̣߳��������߳���������⣬������Ҫ���� ov�����롣
	*/
	if (log != "")
	{
		DWORD bytesToWrite = log.length();
		WriteFile(pThis->_fileHandle, log.c_str(), bytesToWrite, &bytesWrited, &pThis->_pLogContext->ov);
		pThis->_pLogContext->ov.Offset += bytesToWrite;
	}
	log = "";
	return 0;
}

void TraceLog::IocpCallBack(LPVOID lParam)
{
	//IOCP_CONTEXT* pResult = (IOCP_CONTEXT*)lParam;
	//LogContext* p = CONTAINING_RECORD(pResult, LogContext, ov);
	//std::cout << "raceLog::IocpCallBack" << std::endl;
	return;
}

bool TraceLog::InitIocpTask(IOCPService* io_service)
{
	//������־�ļ�
	_logFile = "Log_" + GetTimeString();
	wchar_t* pfilename = multiByteToWideChar(_logFile);
	_fileHandle = CreateFile(pfilename, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED, NULL);
	SAFE_DELETE_ARR(pfilename);
	if (INVALID_HANDLE_VALUE == _fileHandle)
	{
		cout << "������־�ļ�ʧ��,error code:" << GetLastError() << endl;
		return false;
	}
	_pLogContext = new LogContext();
	memset(_pLogContext, 0, sizeof(LogContext));
	//��ʼ���ٽ���
	InitializeCriticalSection(&_cs);
	//ע����־�߳�
	ThreadPool::GetInstance()->AddThreadTask(TraceLogThread, this, MAX_TRHEAD_RUNTIMES, 1);
	//ע��IOCP����
	bool rst = io_service->RegisterHandle(_fileHandle, this);
	if (false == rst)
	{
		cout << "TraceLogע��IOCP����ʧ��" << endl;
		return false;
	}
	_runFlag = true;
	return true;
}

bool TraceLog::RegNewIocpTask(IOCP_CONTEXT *)
{
	return true;
}


bool TraceLog::TRACELOG(stringstream& is, TRACELOG_LEVEL lv)
{
	if (_runFlag == false)
	{
		cout << is.str();
		return false;
	}
	EnterCriticalSection(&_cs);
	_logList.push_back(is.str());
	LeaveCriticalSection(&_cs);
	return true;
}

TraceLog::TraceLog()
{
	_runFlag = false;
}

TraceLog::~TraceLog()
{
	_runFlag = false;
	CloseHandle(_fileHandle);
	SAFE_DELETE(_pLogContext);
}

