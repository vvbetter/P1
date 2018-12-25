#pragma once
#include <Windows.h>
#include <crtdbg.h>
//������������ͷ
struct NetCmd
{
	USHORT length;
	USHORT mainCmd;
	USHORT subCmd;
};

///////  T ��Ҫ�Ǽ̳��� NetCmd �Ľṹ��
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

struct TestNetCmd :public NetCmd
{
	int data;
};