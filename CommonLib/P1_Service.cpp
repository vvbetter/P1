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
	std::cout << "------ ��������ʼ��......-------" << endl;
	timeBeginPeriod(1);
	bool rst = false;
	//��ȡ�����ļ�
	rst = ConfigManager::GetInstance()->ParseConfigFile(serverId);
	if (false == rst)
	{
		std::cout << "���������ļ�ʧ��" << endl;
		return false;
	}
	std::cout << "ConfigManager Init Success" << std::endl;
	//��ʼ���̳߳�
	rst = ThreadPool::GetInstance()->InitThreadPool();
	if (false == rst)
	{
		std::cout << ("ThreadPool Init faided") << endl;
		return rst;
		
	}
	std::cout << ("ThreadPool Init Success") << endl;
	//��ʼ����ʱ��
	rst = TimerManager::GetInstance()->InitTimerManager();
	if (false == rst)
	{
		std::cout << ("TimerManager Init Failed") << endl;
		return rst;
	}
	std::cout << ("TimerManager Init Success") << endl;
	//��ʼ�� IOCPService 
	rst = _io.InitService();
	if (false == rst)
	{
		std::cout << ("IOCPSercie Init faided") << endl;
		return rst;
	}
	std::cout << ("IOCPSercie Init Success") << endl;
	//��ʼ����־����
	rst = TraceLog::GetInstance()->InitIocpTask(&_io);
	if (false == rst)
	{
		std::cout << ("TraceLog Init faided") << endl;
		return rst;
	}
	std::cout << ("TraceLog Init Success") << endl;
	//��ʼ�����������
	rst = _netTasks.InitIocpTask(&_io);
	if (false == rst)
	{
		std::cout << ("NetServiceTask Init Failed") << endl;
		return false;
	}
	std::cout << ("NetServiceTask Init Success") << endl;
	std::cout << "------ ��������ʼ�� �ɹ�!!! -------" << endl;
	return true;
}