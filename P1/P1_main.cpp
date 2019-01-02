#include "P1_Service.h"
#include <iostream>
//#pragma comment(lib,"Ws2_32.lib")
using namespace std;

int main()
{
	P1_Service p1;
	p1.P1_Start();

	int n = 100000;
	while (1)
	{
		Sleep(5000);
	}
	return 0;
}