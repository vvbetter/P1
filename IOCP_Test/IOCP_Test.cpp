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
	AsynLog log(io_service);
	if (io_service.initService())
	{
		log.initAsynLog();
		int n = 100;
		while (n--)
		{
			Log("this is %d \n", n);
		}
	}
	return 0;
}
