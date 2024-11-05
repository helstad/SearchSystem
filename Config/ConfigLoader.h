#pragma once

#include <iostream>
#include <map>
#include <vector>

struct DatabaseConfig
{
	std::string host;
	std::string port;
	std::string dbname;
	std::string user;
	std::string password;

	std::string toConnectionString() const
	{
		return "host=" + host + " port=" + port + " dbname=" + dbname + " user=" + user + " password=" + password;
	}
};

struct SpiderConfig
{
	std::string start_url;
	int depth;
};

struct ServerConfig
{
	int port;
};

struct LinksConfig
{
	std::vector<std::string> urls;
};

struct Config
{
	DatabaseConfig database;
	SpiderConfig spider;
	ServerConfig server;
	LinksConfig links;
};

class ConfigLoader
{
public:
	ConfigLoader() = default;

	bool load();
	const Config& getConfig() const;

private:
	Config config_;
	std::string fileName_ = "config.ini";

	void parseLine(const std::string& line, const std::string& section);
	void loadDatabaseConfig(const std::string& key, const std::string& value);
	void loadSpiderConfig(const std::string& key, const std::string& value);
	void loadServerConfig(const std::string& key, const std::string& value);
	void loadLinksConfig(const std::string& key, const std::string& value);
};

