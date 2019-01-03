#pragma once
#ifndef _P1_INTERFACE
#define _P1_INTERFACE
#include <string>
#include <WinSock2.h>
#include <Windows.h>

constexpr UINT32 SOCKET_BUFFER_SIZE = 8*1024;

class IOCPService; 
class INetServer;
struct NetCmd;

enum NET_OPERATOR
{
	NET_OP_READ = 1,
	NET_OP_WRITE,
};

enum TIMER_STATE
{
	TIMER_WAIT,     //等待运行
	TIMER_RUN,      //立即运行
	TIMER_REMOVE,   //移除
	TIMER_SUSPEND   //挂起暂停
};

struct IOCP_CONTEXT
{
	OVERLAPPED ov;
};
//日志异步结构体
struct LogContext :public IOCP_CONTEXT
{

};
//网络异步结构体
struct NET_CONTEXT : public IOCP_CONTEXT
{
	WSABUF wsaRecvBuf;
	WSABUF wsaSendBuf;
	char buf[SOCKET_BUFFER_SIZE];
	HANDLE s;
	INetServer* pNetServer;
	NET_OPERATOR op;
};
class IIOCPTaskInterface
{
public:
	virtual void IocpCallBack(LPVOID) = 0;
	virtual bool InitIocpTask(IOCPService* io_service) = 0;
	virtual bool RegNewIocpTask(IOCP_CONTEXT*) = 0;
};

class INetServer
{
public:
	virtual bool InitServer(const std::string& ip,const USHORT port) = 0;
	virtual bool NetServerCallBack(NET_CONTEXT*) = 0;
	virtual bool SendCmdData(SOCKET s, NetCmd* pCmd) = 0;
};

//TIMER
typedef void(*TimerCallBack)(LPVOID);
class ITimer
{
public:
	virtual bool InitTimer(IN UINT timerId, IN LPVOID lParam, IN TimerCallBack cb, IN UINT interval, IN UINT64 runcount) = 0;
	virtual UINT GetTimerId() = 0;
	virtual bool ChangeTimerState(IN TIMER_STATE state) = 0;
	virtual TIMER_STATE QueryExcuteTimer() = 0;
	virtual bool ExcuteTimer() = 0;
};
#endif // !_P1_INTERFACE

