#include "stdafx.h"
#include "RWAutoLock.h"


void RWAutoLock::ReadLock()
{
	while (true)
	{
		if (!_WriteRefCount)
		{
			::InterlockedIncrement(&_ReadRefCount);
			if (_WriteRefCount)
			{
				// �ж�д��ͻ����λ��д
				::InterlockedDecrement(&_ReadRefCount);
				continue;
			}
			break;
		}
		::Sleep(0);
	}
}

void RWAutoLock::ReadUnLock()
{
	::InterlockedDecrement(&_ReadRefCount);
}

void RWAutoLock::WriteLock()
{
	::InterlockedIncrement(&_WriteRefCount);
	::EnterCriticalSection(&_cs);
	// �ȴ��Ѿ��ڶ����߳̽���������
	while (true)
	{
		if (!_ReadRefCount)
		{
			break;
		}
		::Sleep(0);
	}
}

void RWAutoLock::WriteUnlock()
{
	LeaveCriticalSection(&_cs);
	::InterlockedDecrement(&_WriteRefCount);
}


RWAutoLock::RWAutoLock():_ReadRefCount(0), _WriteRefCount(0)
{
	InitializeCriticalSection(&_cs);
}

RWAutoLock::~RWAutoLock()
{
	DeleteCriticalSection(&_cs);
}
