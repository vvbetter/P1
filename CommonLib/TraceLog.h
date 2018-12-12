#pragma once
#include "Interface.h"
#include <iostream>
using namespace std;

class TraceLog : public IIOCPTaskInterface
{
public:
	static TraceLog Instance();
	virtual void IocpCallBack(LPVOID);
	virtual bool InitIocpTask();
	const HANDLE GetHanlde() { return _fileHandle; }
private:
	TraceLog();
	~TraceLog();
	TraceLog(const TraceLog&);
	TraceLog& operator= (const TraceLog&);
private:
	const string GetTimeString();
public:
	string _logFile;
	HANDLE _fileHandle;
};

