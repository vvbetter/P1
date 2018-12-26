#pragma once
#include <Windows.h>
#include <crtdbg.h>
//网络命令数据头
struct NetCmd
{
	USHORT length;  //数据结构长度
	USHORT mainCmd; //主命令码
	USHORT subCmd;  //子命令码
};

///////  T 需要是继承至 NetCmd 的结构体
template<class T>
inline T* CreateNetCmd(USHORT mainCmd, USHORT subCmd)
{
	T* pCmd = new T();
	pCmd->mainCmd = mainCmd;
	pCmd->subCmd = subCmd;
	pCmd->length = sizeof(T);
	return pCmd;
}

template<class T>
inline T* CreateNetCmd(LPVOID src)
{
	_ASSERT(((NetCmd*)src)->length == sizeof(T));
	if (((NetCmd*)src)->length != sizeof(T))
	{
		return NULL;
	}
	T* pCmd = new T();
	USHORT length = sizeof(T);
	memcpy_s(pCmd, length, src, length);
	return pCmd;
}

enum MainCmds
{
	CMD_TEST = 0,
};

// mainCmd:1,subCmd:1
struct TestNetCmd :public NetCmd
{
	int data;
};