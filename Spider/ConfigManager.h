#pragma once

#include <string>
#include <map>

class ConfigManager
{
public:
	ConfigManager(const std::string& filename);
	bool load();
	std::string get(const std::string& key) const;

private:
	std::string fileName_;
	std::map<std::string, std::string> configMap_;
};

