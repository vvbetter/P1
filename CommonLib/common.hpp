#pragma once
#include <Windows.h>
#include <string>

#define SAFE_DELETE(x) if(x){delete (x); (x) = NULL;}
#define SAFE_DELETE_ARR(x) if(x){ delete[] (x); (x) = NULL; }


wchar_t *multiByteToWideChar(const std::string& pKey)
{
	const char* pCStrKey = pKey.c_str();
	int pSize = MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, NULL, 0);
	wchar_t *pWCStrKey = new wchar_t[pSize];
	MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, pWCStrKey, pSize);
	return pWCStrKey;
}

char* wideCharToMultiByte(wchar_t* pWCStrKey)
{
	int pSize = WideCharToMultiByte(CP_OEMCP, 0, pWCStrKey, wcslen(pWCStrKey), NULL, 0, NULL, NULL);
	char* pCStrKey = new char[pSize + 1];
	WideCharToMultiByte(CP_OEMCP, 0, pWCStrKey, wcslen(pWCStrKey), pCStrKey, pSize, NULL, NULL);
	pCStrKey[pSize] = '\0';
	return pCStrKey;
}