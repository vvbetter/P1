#pragma once
#include<string>
#include<vector>
#include <map>

const std::string CFG_FILE = "../Config/ServerConfig.json";

struct ServerInfo
{
	std::string ip;
	uint16_t uProt;
};


struct ServerConfig
{
	std::string name;
	std::vector<ServerInfo> tcpServer;
	std::vector<ServerInfo> tcpClient;
};


class ConfigManager
{
public:
	static ConfigManager* GetInstance();
private:
	ConfigManager();
	~ConfigManager();
	ConfigManager(const ConfigManager&);
	ConfigManager& operator= (const ConfigManager&);
//功能实现	
public:
	bool ParseConfigFile(int serverId, const std::string& cfgFile = CFG_FILE);
public:
	const ServerConfig* GetServerCfg();
private:
	std::map<int, ServerConfig> _cfg;
};

