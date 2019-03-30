#include "pch.h"
#include "Clients.h"
#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"Winmm.lib")

int main(int argc,char* argv[])
{
	if (argc != 4)
	{
		cout << "参数错误" << endl;
		return 0;
	}
	cout << "开始连接 " << timeGetTime() << endl;
	int CinetsNum = atoi(argv[1]);
	char* ip = argv[2];
	int port = atoi(argv[3]);

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	bool iResult = true;
	for (int i = 0; i < CinetsNum; ++i)
	{
		Client* pClient = new Client();
		iResult &= pClient->ConnectToServer(ip, port);
		if (iResult == false)
		{
			break;
		}
	}
	cout << "连接完成 " << timeGetTime() << endl;
	while (iResult)
	{
		Sleep(1000000);
	}
	WSACleanup();
	return 0;
}