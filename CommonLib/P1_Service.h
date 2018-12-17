#pragma once
#include "IOCPService.h"
#include "TraceLog.h"
class P1_Service
{
public:
	P1_Service();
	~P1_Service();
public:
	bool P1_Start();

	void P1_Log(std::stringstream& is, TRACELOG_LEVEL lv = LOG_DEBUG);
private:
	IOCPService _io;
	TraceLog _log;
};

