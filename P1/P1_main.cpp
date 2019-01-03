#include "P1_Service.h"
#include <iostream>
using namespace std;

int main(int argv ,char* argc[] )
{
	if (argv != 2)
	{
		cout << "²ÎÊý´íÎó" << endl;
		return -1;
	}
	int serverId = atoi(argc[1]);
	P1_Service service;
	service.P1_Start(serverId);
	while (1)
	{
		Sleep(5000);
	}
	return 0;
}