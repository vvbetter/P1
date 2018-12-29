#include "pch.h"
#include "Clients.h"
#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"Winmm.lib")

int main()
{
	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data);
	ThreadPool::GetInstance()->InitThreadPool(40);

	bool iResult = true;
	for (int i = 0; i < 1000; ++i)
	{
		Client* pClient = new Client();
		iResult &= pClient->ConnectToServer();
		Sleep(100);
		if (iResult == false)
		{
			break;
		}
	}
	while (iResult)
	{
		Sleep(1000000);
	}
	return 0;
}