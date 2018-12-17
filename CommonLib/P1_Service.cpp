#include "stdafx.h"
#include "P1_Service.h"
#include <iostream>

P1_Service::P1_Service()
{
}


P1_Service::~P1_Service()
{
}

bool P1_Service::P1_Start()
{
	//��ʼ�� IOCPService 
	bool rst = false;
	rst = _io.InitService();
	if (false == rst)
	{
		std::cout << "IOCPSercie Init false" << std::endl;
		return rst;
	}
	//��ʼ����־����
	rst = _log.InitIocpTask(&_io);
	if (false == rst)
	{
		std::cout << "TraceLog Init false" << std::endl;
		return rst;
	}
	return false;
}

void P1_Service::P1_Log(std::stringstream& is, TRACELOG_LEVEL lv)
{
	_log.TRACELOG(is, lv);
}
