#include "stdafx.h"
#include "ThreadPool.h"
#include <process.h>

unsigned int __stdcall ThreadPool::ThreadCallBack(LPVOID lParam)
{
	ThreadPool* pThreadPool = (ThreadPool*)lParam;
	std::map<UINT, ThreadTask>& tasks = pThreadPool->_mapTask;
	while (pThreadPool->_runFlag)
	{

	}
	return 0;
}

UINT ThreadPool::_tid = 0;

bool ThreadPool::InitThreadPool(UINT num)
{
	if (0 == num)
	{
		num = 20;//TODO
	}
	_runFlag = true;
	for (UINT i = 0; i < num; ++i)
	{
		HANDLE h = (HANDLE)_beginthreadex(NULL, 0, ThreadCallBack, this, 0, NULL);
		if(h != INVALID_HANDLE_VALUE)
		{
			_threadHandles.push_back(h);
		}
	}
	InitializeCriticalSection(&_cs);
	return true;
}

bool ThreadPool::CloseThreadPool()
{
	_runFlag = false;
	for (auto it = _threadHandles.begin(); it != _threadHandles.end(); ++it)
	{
		HANDLE h = *it;
		WaitForSingleObject(h, INFINITE);
		CloseHandle(h);
	}
	_threadHandles.clear();
	return true;
}

UINT ThreadPool::AddThreadTask(ThreadPoolCallBack cb, LPVOID baseaddr, UINT64 runtimes, bool mulFlag)
{
	if (NULL == cb || NULL == baseaddr)
	{
		return (UINT)-1;
	}
	ThreadTask oneTask;
	oneTask.baseaddr = baseaddr;
	oneTask.cb = cb;
	oneTask.runtimes = runtimes;
	oneTask.mulFlag = mulFlag;
	oneTask.runed = 0;
	UINT tid = 0;
	EnterCriticalSection(&_cs);
	_mapTask[_tid] = oneTask;
	tid = _tid;
	_tid++;
	LeaveCriticalSection(&_cs);
	return tid;
}

bool ThreadPool::RemoveThreadTask(UINT tid)
{
	EnterCriticalSection(&_cs);
	auto it = _mapTask.find(tid);
	if (it != _mapTask.end())
	{
		_mapTask.erase(it);
	}
	LeaveCriticalSection(&_cs);
	return false;
}

ThreadPool::ThreadPool()
{
	_runFlag = false;
}

ThreadPool::~ThreadPool()
{
	CloseThreadPool();
}
