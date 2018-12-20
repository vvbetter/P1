#pragma once
#include <Windows.h>
#include <map>
#include <vector>

constexpr UINT64 MAX_TRHEAD_RUNTIMES = (UINT64)-1;
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
	bool mulFlag;			//多线程并发标志
	UINT runingnum;			//并发数量
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
	UINT AddThreadTask(ThreadPoolCallBack cb, LPVOID param, UINT64 runtimes, bool mulFlag /*是否多线程并发标志*/ = false);
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

