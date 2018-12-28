#pragma once
#include "Interface.h"
#include <map>
constexpr UINT MIN_TIMER_INTERVAL = 10; //定时器最小间隔时间（Milliseconds）
constexpr UINT64 MAX_TIMER_RUNCOUNT = (UINT64)-1; // 定时器运行次数

class CTimer :public ITimer
{
public:
	virtual bool InitTimer(IN UINT timerId, IN LPVOID lParam, IN TimerCallBack cb, IN UINT interval, IN UINT64 runcount);
	virtual UINT GetTimerId() { return _timerId; };
	virtual bool ChangeTimerState(IN TIMER_STATE state); //移除、或者挂起
	virtual TIMER_STATE QueryExcuteTimer();
	virtual bool ExcuteTimer();
public:
	CTimer();
	virtual ~CTimer();
private:
	UINT _timerId;
	TimerCallBack _cb;
	LPVOID _lParam;
	UINT _interval;
	UINT64 _runcount;
	///////////////////
	DWORD _lastExcuteTime;
	TIMER_STATE _state;
};

class TimerManager 
{
public:
	static TimerManager* GetInstance();
	static UINT TimerThread(LPVOID);
public:
	bool InitTimerManager();
	UINT CreateTimer(IN TimerCallBack cb, IN LPVOID lParam, IN UINT interval = MIN_TIMER_INTERVAL, IN UINT64 runcount = MAX_TIMER_RUNCOUNT);
	bool ChangeTimerState(UINT timerId, TIMER_STATE state);
	bool ExcuteTimerTask();
private:
	TimerManager();
	virtual ~TimerManager();
private:
	UINT _timerId;
	std::map<UINT, ITimer*> _timer;
	CRITICAL_SECTION _cs;
	volatile DWORD _lastQueryTime;
};

