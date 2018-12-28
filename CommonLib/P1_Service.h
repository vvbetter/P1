#pragma once
#include "IOCPService.h"
#include "TraceLog.h"
#include "NetServiceTask.h"
class P1_Service
{
public:
	P1_Service();
	~P1_Service();
public:
	bool P1_Start();
private:
	IOCPService _io;
	NetServiceTask _netTasks;
};

