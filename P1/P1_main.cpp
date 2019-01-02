#include "P1_Service.h"
#include <iostream>
#include "ConfigManager.h"
//#pragma comment(lib,"Ws2_32.lib")
using namespace std;

int main()
{

	ConfigManager::GetInstance().ParseConfigFile();

	return 0;
}