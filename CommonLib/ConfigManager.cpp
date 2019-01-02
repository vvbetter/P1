#include "stdafx.h"
#include "ConfigManager.h"
#include<iostream>
#include<boost/property_tree/ptree.hpp>
#include<boost/property_tree/json_parser.hpp>
#include<boost/filesystem.hpp>

using namespace boost::filesystem;
using namespace boost::property_tree;

ConfigManager & ConfigManager::GetInstance()
{
	static ConfigManager manager;
	return manager;
}

ConfigManager::ConfigManager()
{
}


ConfigManager::~ConfigManager()
{
}

bool ConfigManager::ParseConfigFile(const std::string & cfgFile)
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
				ServerConfig config;
				int cfgId = atoi(it->first.c_str());
				auto& cfg = it->second;

				config.name = cfg.get<std::string>("name");
				auto& list = cfg.get_child("list");
				for (auto it = list.begin(); it != list.end(); ++it)
				{
					ServerInfo info;
					info.ip = it->second.get<std::string>("ip");
					info.uProt = it->second.get<uint32_t>("port");
					config.infos.push_back(info);
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
