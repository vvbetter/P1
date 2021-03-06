#include "pch.h"
#include "ThreadPool.h"

ThreadPool::ThreadPool(IO_Service & io): io_service(io)
{
	threadTaskId = 0;
}

ThreadPool::~ThreadPool()
{
}

DWORD ThreadPool::registerThread(ThreadCallBackFunction fn, void * param, DWORD concurrency)
{
	if (concurrency == (DWORD)THREAD_CONCURRENCY_MAX)
	{
		concurrency = io_service.threadSums();
	}
	if (concurrency == 0 || concurrency > io_service.threadSums())
	{
		return false;
	}
	DWORD taskId = ::InterlockedIncrement(&threadTaskId);
	std::vector<THREADPOOL_OVERLAPPED*> pOvs;
	for (DWORD i = 0; i < concurrency; ++i)
	{
		THREADPOOL_OVERLAPPED* pov = new THREADPOOL_OVERLAPPED();
		pov->fn = fn;
		pov->param = param;
		pov->taskState = 1;
		pov->taskId = taskId;
		pOvs.push_back(pov);
		io_service.postIocpTask(pov, this, 0);
	}
	{
		std::lock_guard<std::mutex> locker(mutex);
		threadTasks[taskId] = pOvs;
	}
	return taskId;
}

bool ThreadPool::unRegisterThread(DWORD threadId)
{
	std::lock_guard<std::mutex> locker(mutex);
	auto taskIt = threadTasks.find(threadId);
	if (taskIt == threadTasks.end())
	{
		return false;
	}
	for (auto it = taskIt->second.begin(); it != taskIt->second.end(); ++it)
	{
		(*it)->taskState = 0;
	}

	return true;
}

bool ThreadPool::callBackFunction(IO_OVERLAPPED * pOv, DWORD NumberOfBytesTransferred)
{
	THREADPOOL_OVERLAPPED* pIoov = CONTAINING_RECORD(pOv, THREADPOOL_OVERLAPPED, ov);
	pIoov->fn(pIoov->param);
	if (pIoov->taskState == 0)
	{
		std::lock_guard<std::mutex> locker(mutex);
		DWORD taskId = pIoov->taskId;
		auto taskIt = threadTasks.find(taskId);
		if (taskIt == threadTasks.end())
		{
			return false;
		}
		DWORD state = 0;
		for (auto it = taskIt->second.begin(); it != taskIt->second.end(); ++it)
		{
			if ((*it) == pIoov)
			{
				delete (*it);
				(*it) = NULL;
			}
			if ((*it) != NULL)
			{
				++state;
			}
		}
		if (state == 0)
		{
			std::cout << "Erase Thread Task " << taskIt->first << std::endl;
			threadTasks.erase(taskIt);
		}
	}
	else
	{
		io_service.postIocpTask(pIoov, this, 0);
	}
	return true;
}

bool ThreadPool::shutDown()
{
	return false;
}
