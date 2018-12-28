#include "stdafx.h"
#include "TimerManager.h"
#include "common.hpp"
#include "ThreadPool.h"

bool CTimer::InitTimer(IN UINT timerId, IN LPVOID lParam, IN TimerCallBack cb, IN UINT interval, IN UINT64 runcount)
{
	_timerId = timerId;
	_cb = cb;
	_lParam = lParam;
	_interval = interval;
	_runcount = runcount;
	_lastExcuteTime = 0;
	_state = TIMER_WAIT;
	return true;
}

bool CTimer::ChangeTimerState(IN TIMER_STATE state)
{
	_state = state;
	return true;
}

TIMER_STATE CTimer::QueryExcuteTimer()
{
	DWORD tick = timeGetTime();
	switch (_state)
	{
	case TIMER_REMOVE:
		return TIMER_REMOVE;
	case TIMER_SUSPEND:
		return TIMER_SUSPEND;
	case TIMER_WAIT:
	{
		if (_lastExcuteTime + _interval < tick)
		{
			_lastExcuteTime = tick;
			return TIMER_RUN;
		}
		else
		{
			return TIMER_WAIT;
		}
	}
	case TIMER_RUN:
		_state = TIMER_WAIT;
		return TIMER_RUN;
	default:
		return TIMER_REMOVE;
	}
	return TIMER_REMOVE;
}

bool CTimer::ExcuteTimer()
{
	_cb(_lParam);
	return true;
}

CTimer::CTimer()
{
}

CTimer::~CTimer()
{
}


TimerManager * TimerManager::GetInstance()
{
	static TimerManager manager;
	return &manager;
}

UINT TimerManager::TimerThread(LPVOID lParam)
{
	TimerManager* pManager = (TimerManager*)lParam;
	DWORD tick = timeGetTime();
	if (tick - pManager->_lastQueryTime < MIN_TIMER_INTERVAL)
	{
		return 0;
	}
	pManager->ExcuteTimerTask();
	pManager->_lastQueryTime = tick;
	return 0;
}

bool TimerManager::InitTimerManager()
{
	_lastQueryTime = timeGetTime();
	_timerId = 0;
	InitializeCriticalSection(&_cs);
	ThreadPool::GetInstance()->AddThreadTask(TimerThread, this, MAX_THREAD_RUNNING, 2);
	return true;
}

UINT TimerManager::CreateTimer(IN TimerCallBack cb, IN LPVOID lParam, IN UINT interval, IN UINT64 runcount)
{
	UINT timerId = ::InterlockedIncrement(&_timerId);
	CTimer* pTimer = new CTimer();
	pTimer->InitTimer(timerId, lParam, cb, interval, runcount);
	EnterCriticalSection(&_cs);
	_timer[timerId] = pTimer;
	LeaveCriticalSection(&_cs);
	return timerId;
}

bool TimerManager::ChangeTimerState(UINT timerId, TIMER_STATE state)
{
	bool iResult = false;
	EnterCriticalSection(&_cs);
	auto it = _timer.find(timerId);
	if (it != _timer.end())
	{
		iResult = it->second->ChangeTimerState(state);
	}
	LeaveCriticalSection(&_cs);
	return  iResult;
}

bool TimerManager::ExcuteTimerTask()
{
	DWORD tick = timeGetTime();
	ITimer* pTimer = NULL;
	EnterCriticalSection(&_cs);
	for (auto it = _timer.begin(); it != _timer.end();)
	{
		TIMER_STATE state = it->second->QueryExcuteTimer();
		switch (state)
		{
		case TIMER_WAIT:
		{
			break;
		}
		case TIMER_RUN:
		{
			pTimer = it->second;
			break;
		}
		case TIMER_REMOVE:
		{
			ITimer* timer = it->second;
			SAFE_DELETE(timer);
			it = _timer.erase(it);
			break;
		}
		case TIMER_SUSPEND:
		{
			break;
		}
		default:
			break;
		}
		if (NULL != pTimer)
		{
			break;
		}
		++it;
	}
	LeaveCriticalSection(&_cs);
	if (pTimer != NULL)
	{
		pTimer->ExcuteTimer();
	}
	return true;
}

TimerManager::TimerManager()
{
	
}

TimerManager::~TimerManager()
{
	DeleteCriticalSection(&_cs);
}


