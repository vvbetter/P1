#include "pch.h"
#include "Clients.h"
#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"Winmm.lib")

int main(int argc,char* argv[])
{
	if (argc != 2)
	{
		cout << "²ÎÊý´íÎó" << endl;
		return 0;
	}
	int CinetsNum = atoi(argv[1]);

	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data);
	ThreadPool::GetInstance()->InitThreadPool(20);

	bool iResult = true;
	for (int i = 0; i < CinetsNum; ++i)
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