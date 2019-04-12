#pragma once
#ifndef __COMMMON_HPP__
#define __COMMMON_HPP__
#include <string>
#include <iostream>
#include <sstream>
#include <time.h>
#include <WinSock2.h>
#include <Windows.h>
#pragma comment(lib,"Ws2_32.lib")


#define SAFE_DELETE(x) if(x){delete (x); (x) = NULL;}
#define SAFE_DELETE_ARR(x) if(x){ delete[] (x); (x) = NULL; }

inline wchar_t *multiByteToWideChar(const std::string& pKey)
{
	const char* pCStrKey = pKey.c_str();
	int pSize = MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, NULL, 0);
	wchar_t *pWCStrKey = new wchar_t[pSize];
	MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, pWCStrKey, pSize);
	return pWCStrKey;
}

inline char* wideCharToMultiByte(wchar_t* pWCStrKey)
{
	int pSize = WideCharToMultiByte(CP_OEMCP, 0, pWCStrKey, wcslen(pWCStrKey), NULL, 0, NULL, NULL);
	char* pCStrKey = new char[pSize + 1];
	WideCharToMultiByte(CP_OEMCP, 0, pWCStrKey, wcslen(pWCStrKey), pCStrKey, pSize, NULL, NULL);
	pCStrKey[pSize] = '\0';
	return pCStrKey;
}

inline static std::string GetTimeString()
{
	SYSTEMTIME st;
	GetSystemTime(&st);
	std::stringstream ss;
	ss.width(4);
	ss.fill('0');
	ss << st.wYear;
	ss.width(2);
	ss.fill('0');
	ss << st.wMonth;
	ss.width(2);
	ss.fill('0');
	ss << st.wDay;
	ss.width(2);
	ss.fill('0');
	ss << st.wHour + 8;
	ss.width(2);
	ss.fill('0');
	ss << st.wMinute;
	ss.width(2);
	ss.fill('0');
	ss << st.wSecond;
	return ss.str();
}
inline const std::string GetTimeStr()
{
	time_t NowTime = time(NULL);
	tm pNowTime;
	errno_t Error = localtime_s(&pNowTime, &NowTime);
	std::stringstream ss;
	ss << pNowTime.tm_year + 1900 << "-" << pNowTime.tm_mon + 1 << "-" << pNowTime.tm_mday << " " << pNowTime.tm_hour << ":" << pNowTime.tm_min << ":" << pNowTime.tm_sec << " ";
	return ss.str();
}
void Log(const CHAR *pcStr, ...);
#endif // !__COMMMON_HPP__