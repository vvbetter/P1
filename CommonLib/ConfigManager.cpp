#include "stdafx.h"
#include "ConfigManager.h"
#include<iostream>
#include<boost/property_tree/ptree.hpp>
#include<boost/property_tree/json_parser.hpp>
#include<boost/filesystem.hpp>

using namespace boost::filesystem;
using namespace boost::property_tree;

ConfigManager & ConfigManager::Instance()
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
			ptree ServerNode = rootNode.get_child("Server");
			_serverInfo.ip = ServerNode.get<std::string>("ip");
			_serverInfo.uProt = ServerNode.get<uint16_t>("port");
		}
		catch (ptree_error& e)
		{
			std::cout << "解析配置文件错误：" << e.what() << std::endl;
		}
		catch (...)
		{
			std::cout << "ParseConfigFile Error" << std::endl;
		}
	}
	else
	{
		std::cout << "配置文件不存在：" << cfgFile << std::endl;
	}
	return false;
}
