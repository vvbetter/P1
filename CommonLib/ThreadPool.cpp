#include "stdafx.h"
#include "ThreadPool.h"
#include <process.h>
#include <timeapi.h>

ThreadPool * ThreadPool::GetInstance()
{
	static ThreadPool threadpool;
	return &threadpool;
}

unsigned int __stdcall ThreadPool::ThreadCallBack(LPVOID lParam)
{
	ThreadPool* pThreadPool = (ThreadPool*)lParam;
	std::map<UINT, ThreadTask>& tasks = pThreadPool->_mapTask;
	while (pThreadPool->_runFlag)
	{
		DWORD tick = timeGetTime();
		const ThreadTask task = pThreadPool->GetThreadTask();
		if (NULL != task.cb && NULL != task.param)
		{
			//回调线程
			task.cb(task.param);
			//执行完成之后设置状态
			DWORD endtick = timeGetTime();
			EnterCriticalSection(&pThreadPool->_cs);
			ThreadTask& t = tasks[task.tid];
			t.runingnum > 0 ? --t.runingnum : 0;
			t.lastruntime = endtick;
			t.runtime = (endtick != tick) ? (endtick - tick) : 1;
			LeaveCriticalSection(&pThreadPool->_cs);
		}
		else
		{
			Sleep(1);
		}
	}
	return 0;
}

UINT ThreadPool::_tid = 0;

bool ThreadPool::InitThreadPool(UINT num)
{
	if (0 == num)
	{
		num = 20;//TODO 设置线程池线程数量
	}
	_runFlag = true;
	InitializeCriticalSection(&_cs);
	for (UINT i = 0; i < num; ++i)
	{
		HANDLE h = (HANDLE)_beginthreadex(NULL, 0, ThreadCallBack, this, 0, NULL);
		if(h != INVALID_HANDLE_VALUE)
		{
			_threadHandles.push_back(h);
		}
	}
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
	DeleteCriticalSection(&_cs);
	return true;
}

UINT ThreadPool::AddThreadTask(ThreadPoolCallBack cb, LPVOID param, UINT64 runtimes, UINT mulNum)
{
	if (NULL == cb || NULL == param)
	{
		return (UINT)-1;
	}
	ThreadTask oneTask;
	oneTask.param = param;
	oneTask.cb = cb;
	oneTask.runtimes = runtimes;
	oneTask.mulNum = mulNum;
	oneTask.runingnum = 0;
	oneTask.runtime = 1;
	oneTask.lastruntime = timeGetTime();
	UINT tid = 0;
	EnterCriticalSection(&_cs);
	oneTask.tid = _tid;
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

const ThreadTask ThreadPool::GetThreadTask()
{
	ThreadTask task;
	task.cb = NULL;
	task.param = NULL;
	double min = 1.0;
	UINT tempTid = (UINT)-1;
	DWORD tick = timeGetTime();
	EnterCriticalSection(&_cs);
	for (auto it = _mapTask.begin(); it != _mapTask.end(); ++it)
	{
		const ThreadTask& t = it->second;
		if ((t.runingnum < t.mulNum) && t.runtimes > 0)
		{
			double timeLv = (tick - t.lastruntime) *1.0 / (tick - t.lastruntime + t.runtime);
			if (timeLv < min)
			{
				min = timeLv;
				tempTid = it->first;
			}
		}
	}
	if (tempTid != (UINT)-1)
	{
		ThreadTask&  t= _mapTask[tempTid];
		++t.runingnum;
		if (t.runtimes != MAX_TRHEAD_RUNTIMES && t.runtimes > 0)
		{
			--t.runtimes;
		}
		task = t;
	}
	LeaveCriticalSection(&_cs);
	return task;
}

ThreadPool::ThreadPool()
{
	_runFlag = false;
}

ThreadPool::~ThreadPool()
{
	CloseThreadPool();
}
