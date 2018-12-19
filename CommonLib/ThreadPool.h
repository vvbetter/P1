#pragma once
#include <Windows.h>
#include <map>
#include <vector>

constexpr UINT64 MAX_TRHEAD_RUNTIMES = (UINT64)-1;
typedef UINT(*ThreadPoolCallBack)(LPVOID);

struct ThreadTask
{
	LPVOID baseaddr;		
	ThreadPoolCallBack cb;	//�ص�
	UINT64 runtimes;		//���д���
	bool mulFlag;			//���̲߳�����־
	UINT runed;				//��������
};

class ThreadPool
{
public:
	static unsigned int __stdcall ThreadCallBack(LPVOID);
	bool InitThreadPool(UINT num = 0);
	bool CloseThreadPool();
	UINT AddThreadTask(ThreadPoolCallBack cb, LPVOID baseaddr, UINT64 runtimes, bool mulFlag /*�Ƿ���̲߳�����־*/ = false);
	bool RemoveThreadTask(UINT tid);
public:
	ThreadPool();
	~ThreadPool();
private:
	static UINT _tid;
	bool _runFlag;
	std::map<UINT, ThreadTask> _mapTask;
	std::vector<HANDLE> _threadHandles;
	CRITICAL_SECTION _cs;
};

