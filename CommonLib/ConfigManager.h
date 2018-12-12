#pragma once
#include<string>

const std::string CFG_FILE = "../Config/ServerConfig.json";

struct ServerInfo
{
	std::string ip;
	uint16_t uProt;
};

class ConfigManager
{
public:
	static ConfigManager& Instance();
private:
	ConfigManager();
	~ConfigManager();
	ConfigManager(const ConfigManager&);
	ConfigManager& operator= (const ConfigManager&);
//功能实现	
public:
	bool ParseConfigFile(const std::string& cfgFile);
public:
	const ServerInfo& GetServerInfo() { return _serverInfo; }
private:
	ServerInfo _serverInfo;
	
};

