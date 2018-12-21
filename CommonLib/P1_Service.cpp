#include "stdafx.h"
#include "P1_Service.h"
#include "common.hpp"
#include "ThreadPool.h"
#include <timeapi.h>
#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"Winmm.lib")

P1_Service::P1_Service()
{
}


P1_Service::~P1_Service()
{
	timeEndPeriod(1);
}

bool P1_Service::P1_Start()
{
	timeBeginPeriod(1);
	bool rst = false;
	//初始化线程池
	rst = ThreadPool::GetInstance()->InitThreadPool();
	if (false == rst)
	{
		P1_LOG("ThreadPool Init faided")
	}
	P1_LOG("ThreadPool Init Success");
	//初始化 IOCPService 
	rst = _io.InitService();
	if (false == rst)
	{
		P1_LOG("IOCPSercie Init faided");
		return rst;
	}
	P1_LOG("IOCPSercie Init Success");
	//初始化日志任务
	rst = _log.InitIocpTask(&_io);
	if (false == rst)
	{
		P1_LOG("TraceLog Init faided");
		return rst;
	}
	P1_LOG("TraceLog Init Success");

	rst = _netTasks.InitIocpTask(&_io);
	if (false == rst)
	{
		P1_LOG("NetServiceTask Init Failed");
		return false;
	}
	P1_LOG("NetServiceTask Init Success")
	return true;
}

void P1_Service::P1_Log(std::stringstream& is, TRACELOG_LEVEL lv)
{
	_log.TRACELOG(is, lv);
}
