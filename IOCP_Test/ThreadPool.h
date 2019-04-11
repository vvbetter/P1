#pragma once
#include "typedef.h"
#include "IO_Service.h"
#include <functional>
#include <map>
#include <vector>
#include <mutex>

#define THREAD_CONCURRENCY_MAX -1

typedef std::function<bool(void*)> ThreadCallBackFunction;

struct THREADPOOL_OVERLAPPED :public IO_OVERLAPPED
{
	ThreadCallBackFunction fn;
	void* param;
	DWORD taskId;
	DWORD taskState;
};

class ThreadPool:public IO_COMPLETIONKEY
{
public:
	ThreadPool(IO_Service& io);
	~ThreadPool();
public:
	DWORD registerThread(ThreadCallBackFunction fn,void* param,  DWORD concurrency);
	bool unRegisterThread(DWORD threadId);
public:
	virtual bool callBackFunction(IO_OVERLAPPED* pOv, DWORD NumberOfBytesTransferred);
	virtual bool shutDown();
private:
	IO_Service& io_service;
	DWORD threadTaskId;
	std::map<DWORD, std::vector<THREADPOOL_OVERLAPPED*> > threadTasks;
	std::mutex mutex;
};
