// IOCP_Test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include "IO_Service.h"
#include "AsynLog.h"
#include "ThreadPool.h"
#include "UdpServer.h"
#include <iostream>
#include <vector>
#include <sstream>
using namespace std;

int main()
{
	cout << "--- START TEST ---" << endl;
	IO_Service io_service;
	if (io_service.initService())
	{
		AsynLog log(io_service);
		log.initAsynLog();
		UdpServer s(io_service);
		s.initServer(50001);
		Sleep(1000000);
	}
	return 0;
}
