// IOCP_Test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include "IO_Service.h"
#include "AsynLog.h"
#include "ThreadPool.h"
#include <iostream>
#include <vector>
#include <sstream>
using namespace std;

class Test
{
public:
	bool t1(void *) {n = ::InterlockedIncrement(&n);  return true; }
	long n;
	Test()
	{
		n = 0;
	}
	void print()
	{
		cout << n << endl;
	}
};


int main()
{
	IO_Service io_service;
	if (io_service.initService())
	{
		AsynLog log(io_service);
		log.initAsynLog();
		
		ThreadPool threadpool(io_service);

		Test* t = new Test();
		ThreadCallBackFunction fn = bind(&Test::t1, t, placeholders::_1);
		DWORD id = threadpool.registerThread(fn, nullptr, 1);
		Sleep(20000);
		threadpool.unRegisterThread(id);
		Sleep(1000);
		t->print();
	}
	Sleep(2000);
	io_service.shutDown();
	return 0;
}
