#include "stdafx.h"
#include "ConfigManager.h"
#include<iostream>
#include<boost/property_tree/ptree.hpp>
#include<boost/property_tree/json_parser.hpp>
#include<boost/filesystem.hpp>

using namespace boost::filesystem;
using namespace boost::property_tree;

ConfigManager* ConfigManager::GetInstance()
{
	static ConfigManager manager;
	return& manager;
}

ConfigManager::ConfigManager()
{
}


ConfigManager::~ConfigManager()
{
}

bool ConfigManager::ParseConfigFile(int serverId, const std::string & cfgFile)
{
	boost::filesystem::path filePath(cfgFile);
	if (is_regular_file(filePath))
	{
		try
		{
			ptree rootNode;
			read_json(cfgFile, rootNode);
			for (auto it = rootNode.begin(); it != rootNode.end(); ++it)
			{
				int cfgId = atoi(it->first.c_str());
				if (cfgId != serverId)
				{
					continue;
				}
				auto& cfg = it->second;
				ServerConfig config;
				config.name = cfg.get<std::string>("name");
				auto& TcpServer = cfg.get_child("TcpServer");
				for (auto it = TcpServer.begin(); it != TcpServer.end(); ++it)
				{
					ServerInfo info;
					info.ip = it->second.get<std::string>("ip");
					info.uProt = it->second.get<uint32_t>("port");
					config.tcpServer.push_back(info);
				}
				auto& TcpClient = cfg.get_child("TcpClient");
				for (auto it = TcpClient.begin(); it != TcpClient.end(); ++it)
				{
					ServerInfo info;
					info.ip = it->second.get<std::string>("ip");
					info.uProt = it->second.get<uint32_t>("port");
					config.tcpClient.push_back(info);
				}
				_cfg[cfgId] = config;
			}
		}
		catch (ptree_error& e)
		{
			std::cout << "解析配置文件错误：" << e.what() << std::endl;
			return false;
		}
		catch (...)
		{
			std::cout << "ParseConfigFile Error" << std::endl;
			return false;
		}
	}
	else
	{
		std::cout << "配置文件不存在：" << cfgFile << std::endl;
		return false;
	}
	return true;
}

const ServerConfig* ConfigManager::GetServerCfg()
{
	assert(_cfg.size() == 1);
	if (_cfg.size() != 1)
	{
		return NULL;
	}
	return &(_cfg.begin()->second);
}
