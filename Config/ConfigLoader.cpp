#include "ConfigLoader.h"

#include <filesystem>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <functional>

bool ConfigLoader::load()
{
	if (!std::filesystem::exists(fileName_))
	{
		std::cerr << "Configuration file not found: " << fileName_ << std::endl;
		return false;
	}

	std::ifstream file(fileName_);
	if (!file.is_open())
	{
		std::cerr << "Failed to open configuration file: " << fileName_ << std::endl;
		return false;
	}

	std::string line;
	std::string currentSection;

	while (std::getline(file, line))
	{
		if (line.empty() || line[0] == '#') continue;

		if (line[0] == '[')
		{
			currentSection = line.substr(1, line.find(']') - 1);
			continue;
		}

		parseLine(line, currentSection);
	}

	file.close();

	return true;
}

const Config& ConfigLoader::getConfig() const
{
	return config_;
}

void ConfigLoader::parseLine(const std::string& line, const std::string& section)
{
	std::istringstream iss(line);
	std::string key, value;
	if (std::getline(iss, key, '=') && std::getline(iss, value))
	{
		key.erase(std::remove_if(key.begin(), key.end(), ::isspace), key.end());
		value.erase(std::remove_if(value.begin(), value.end(), ::isspace), value.end());

		if (section == "Database") loadDatabaseConfig(key, value);
		if (section == "Spider") loadSpiderConfig(key, value);
		if (section == "Server") loadServerConfig(key, value);
	}
}

void ConfigLoader::loadDatabaseConfig(const std::string& key, const std::string& value)
{
	static const std::map<std::string, std::function<void(const std::string&)>> loadMap = {
	{"db_host", [this](const std::string& v) { config_.database.host = v; }},
	{"db_port", [this](const std::string& v) { config_.database.port = v; }},
	{"db_name", [this](const std::string& v) { config_.database.dbname = v; }},
	{"db_user", [this](const std::string& v) { config_.database.user = v; }},
	{"db_password", [this](const std::string& v) { config_.database.password = v; }}
	};

	if (loadMap.count(key))
	{
		loadMap.at(key)(value);
	}
}

void ConfigLoader::loadSpiderConfig(const std::string& key, const std::string& value)
{
	static const std::map<std::string, std::function<void(const std::string&)>> loadMap = {
		{"start_url", [this](const std::string& v) { config_.spider.start_url = v; }},
		{"depth", [this](const std::string& v)
		{
			try
			{
				config_.spider.depth = std::stoi(v);
			}
			catch (const std::invalid_argument&)
			{
				std::cerr << "Invalid depth value: " << v << std::endl;
			}
		}}
	};

	if (loadMap.count(key))
	{
		loadMap.at(key)(value);
	}
}

void ConfigLoader::loadServerConfig(const std::string& key, const std::string& value)
{
	static const std::map<std::string, std::function<void(const std::string&)>> loadMap = {
		{"port", [this](const std::string& v) {
			try
			{
				config_.server.port = std::stoi(v);
			}
			catch (const std::invalid_argument&) 
			{
				std::cerr << "Invalid port value: " << v << std::endl;
			}
		}}
	};

	if (loadMap.count(key))
	{
		loadMap.at(key)(value);
	}
}
