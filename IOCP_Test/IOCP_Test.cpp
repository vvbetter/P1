// IOCP_Test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include "IO_Service.h"
#include "AsynLog.h"
#include <iostream>
#include <vector>
#include <sstream>
using namespace std;
int main()
{
	IO_Service io_service;
	if (io_service.initService())
	{
		AsynLog log(io_service);
		log.initAsynLog();
		string data = "";

		for (int i = 0; i < 20; ++i)
		{
			data += "a";
			string buf = data + "\n";
			//cout << buf << buf.size();
			log.traceLog(buf.c_str(), buf.length());
		}
		Sleep(2000);
	}
	io_service.shutDown();
	return 0;
}
