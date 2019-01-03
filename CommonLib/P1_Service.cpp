#include "stdafx.h"
#include "ConfigManager.h"
#include "P1_Service.h"
#include "common.hpp"
#include "ThreadPool.h"
#include "TimerManager.h"
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

bool P1_Service::P1_Start(int serverId)
{
	std::cout << "------ 服务器初始化......-------" << endl;
	timeBeginPeriod(1);
	bool rst = false;
	//读取配置文件
	rst = ConfigManager::GetInstance()->ParseConfigFile(serverId);
	if (false == rst)
	{
		std::cout << "解析配置文件失败" << endl;
		return false;
	}
	std::cout << "ConfigManager Init Success" << std::endl;
	//初始化线程池
	rst = ThreadPool::GetInstance()->InitThreadPool();
	if (false == rst)
	{
		std::cout << ("ThreadPool Init faided") << endl;
		return rst;
		
	}
	std::cout << ("ThreadPool Init Success") << endl;
	//初始化定时器
	rst = TimerManager::GetInstance()->InitTimerManager();
	if (false == rst)
	{
		std::cout << ("TimerManager Init Failed") << endl;
		return rst;
	}
	std::cout << ("TimerManager Init Success") << endl;
	//初始化 IOCPService 
	rst = _io.InitService();
	if (false == rst)
	{
		std::cout << ("IOCPSercie Init faided") << endl;
		return rst;
	}
	std::cout << ("IOCPSercie Init Success") << endl;
	//初始化日志任务
	rst = TraceLog::GetInstance()->InitIocpTask(&_io);
	if (false == rst)
	{
		std::cout << ("TraceLog Init faided") << endl;
		return rst;
	}
	std::cout << ("TraceLog Init Success") << endl;
	//初始化网络服务器
	rst = _netTasks.InitIocpTask(&_io);
	if (false == rst)
	{
		std::cout << ("NetServiceTask Init Failed") << endl;
		return false;
	}
	std::cout << ("NetServiceTask Init Success") << endl;
	std::cout << "------ 服务器初始化 成功!!! -------" << endl;
	return true;
}