#pragma once
enum IO_OPERATOR
{
	IO_SHUTDOWN = -100,
	IO_DEFAULT = 0,
	IO_READ = 1,
	IO_WRITE = 2,
};

struct IO_OVERLAPPED
{
	IO_OVERLAPPED()
	{
		memset(&ov, 0, sizeof(OVERLAPPED));
		op = IO_DEFAULT;
	}
	OVERLAPPED ov;
	IO_OPERATOR op;
};
/*
class IO_TaskInterface
{
public:
	virtual bool callBackFunction(IO_OVERLAPPED* pOv, DWORD NumberOfBytesTransferred) = 0;
};
*/
struct IO_COMPLETIONKEY
{
	virtual bool callBackFunction(IO_OVERLAPPED* pOv, DWORD NumberOfBytesTransferred) = 0;
	virtual bool shutDown() = 0;
};