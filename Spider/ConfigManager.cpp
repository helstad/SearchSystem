#include "ConfigManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

ConfigManager::ConfigManager(const std::string& fileName) : fileName_(fileName)
{
}

bool ConfigManager::load()
{
	std::cout << "Текущая директория: " << std::filesystem::current_path().string() << std::endl;

	std::ifstream file(fileName_);
	if (!file.is_open())
	{
		std::cerr << "Не удалось открыть файл конфигурации: " << fileName_ << std::endl;
		return false;
	}
	
	std::string line;
	while (std::getline(file, line))
	{
		if (line.empty() || line[0] == '#')
		{
			continue;
		}

		std::istringstream iss(line);
		std::string key, value;
		if (std::getline(iss, key, '=') && std::getline(iss, value))
		{
			key.erase(key.find_last_not_of(" \n\r\t") + 1);
			value.erase(0, value.find_first_not_of(" \n\r\t"));
			configMap_[key] = value;
		}
	}

	file.close();

	return true;
}

std::string ConfigManager::get(const std::string& key) const
{
	auto it = configMap_.find(key);
	if (it != configMap_.end())
	{
		return it->second;
	}

	return "";
}
