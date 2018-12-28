#pragma once
#include "Interface.h"
#include <iostream>
#include <sstream>
#include <list>
using namespace std;

enum TRACELOG_LEVEL
{
	LOG_DEBUG,
	LOG_WARNING,
	LOG_ERROR
};

struct LogContext :public IOCP_CONTEXT
{

};

class TraceLog : public IIOCPTaskInterface
{
public:
	static TraceLog* GetInstance();
	static UINT TraceLogThread(LPVOID);
	virtual void IocpCallBack(LPVOID);
	virtual bool InitIocpTask(IOCPService* io_service);
	virtual bool RegNewIocpTask(IOCP_CONTEXT*);

	bool TRACELOG(stringstream& is, TRACELOG_LEVEL lv = LOG_DEBUG);
	const HANDLE GetHanlde() { return _fileHandle; }
private:
	TraceLog();
	virtual ~TraceLog();
private:
	bool _runFlag;
	string _logFile;
	HANDLE _fileHandle;
	CRITICAL_SECTION _cs;
	list<string> _logList;
	LogContext * _pLogContext;
};

