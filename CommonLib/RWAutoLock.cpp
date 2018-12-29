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
				// 有读写冲突，让位于写
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
	// 等待已经在读的线程结束读操作
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
