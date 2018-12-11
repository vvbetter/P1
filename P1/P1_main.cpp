#include <iostream>
#include <string>
#include "ConfigManager.h"
using namespace std;

int main()
{
	ConfigManager::Instance().ParseConfigFile(CFG_FILE);
	cout << ConfigManager::Instance().GetServerInfo().ip << endl;
	return 0;
}