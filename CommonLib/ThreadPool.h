#pragma once
#include <Windows.h>
#include <map>
#include <vector>

constexpr UINT64 MAX_TRHEAD_RUNTIMES = (UINT64)-1; //线程最大运行次数。设置此值线程将不会扣减运行次数
constexpr UINT MAX_THREAD_RUNNING = (UINT)-1; //任务并行运行最大线程数量。

typedef UINT(*ThreadPoolCallBack)(LPVOID);

struct ThreadTask
{
	ThreadTask()
	{
		tid = (UINT)-1;
		param = NULL;
		cb = NULL;
	}
	UINT tid;
	LPVOID param;		
	ThreadPoolCallBack cb;	//回调
	UINT64 runtimes;		//运行次数
	UINT mulNum;			//最大并发数量
	UINT runingnum;			//当前并发数量
	UINT runtime;			//运行时间
	DWORD lastruntime;		//上次运行完成时间戳
};

class ThreadPool
{
public:
	static ThreadPool* GetInstance();
	static unsigned int __stdcall ThreadCallBack(LPVOID);
	bool InitThreadPool(UINT num = 0);
	bool CloseThreadPool();
	UINT AddThreadTask(ThreadPoolCallBack cb, LPVOID param, UINT64 runtimes, UINT mulNum /*同时运行任务的线程数量*/ = MAX_THREAD_RUNNING);
	bool RemoveThreadTask(UINT tid);
private:
	const ThreadTask GetThreadTask();
private:
	ThreadPool();
	~ThreadPool();
private:
	static UINT _tid;
	bool _runFlag;
	std::map<UINT, ThreadTask> _mapTask;
	std::vector<HANDLE> _threadHandles;
	CRITICAL_SECTION _cs;
};

