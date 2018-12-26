#pragma once
#include <Windows.h>
#include <map>
#include <vector>

constexpr UINT64 MAX_TRHEAD_RUNTIMES = (UINT64)-1; //�߳�������д��������ô�ֵ�߳̽�����ۼ����д���
constexpr UINT MAX_THREAD_RUNNING = (UINT)-1; //��������������߳�������

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
	ThreadPoolCallBack cb;	//�ص�
	UINT64 runtimes;		//���д���
	UINT mulNum;			//��󲢷�����
	UINT runingnum;			//��ǰ��������
	UINT runtime;			//����ʱ��
	DWORD lastruntime;		//�ϴ��������ʱ���
};

class ThreadPool
{
public:
	static ThreadPool* GetInstance();
	static unsigned int __stdcall ThreadCallBack(LPVOID);
	bool InitThreadPool(UINT num = 0);
	bool CloseThreadPool();
	UINT AddThreadTask(ThreadPoolCallBack cb, LPVOID param, UINT64 runtimes, UINT mulNum /*ͬʱ����������߳�����*/ = MAX_THREAD_RUNNING);
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

