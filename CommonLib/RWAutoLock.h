#pragma once
#include <Windows.h>
struct RW_CS
{

};

class RWAutoLock
{
public:
	//interface
	void ReadLock();
	void ReadUnLock();
	void WriteLock();
	void WriteUnlock();
public:
	RWAutoLock();
	~RWAutoLock();

private:
	CRITICAL_SECTION _cs;
	volatile LONG _ReadRefCount;
	volatile LONG _WriteRefCount;
};

