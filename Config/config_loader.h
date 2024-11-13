#pragma once

#include <iostream>
#include <map>
#include <vector>

struct database_config {
	std::string host;
	std::string port;
	std::string dbname;
	std::string user;
	std::string password;

	[[nodiscard]] std::string to_connection_string() const {
		return "host=" + host + " port=" + port + " dbname=" + dbname +
			" user=" + user + " password=" + password;
	}
};

struct spider_config {
	std::string start_url;
	int depth{};
};

struct server_config {
	int port;
};

struct links_config {
	std::vector<std::string> urls;
};

struct config {
	database_config database;
	spider_config spider;
	server_config server;
	links_config links;
};

class config_loader {
public:
	config_loader() = default;

	bool load();
	auto get_config() const -> const config&;
	auto get_urls() const -> std::vector<std::string>;

private:
	config config_;
	std::string file_name_ = "config.ini";

	void parse_line( const std::string& line, const std::string& section );
	void load_database_config( const std::string& key, const std::string& value );
	void load_spider_config( const std::string& key, const std::string& value );
	void load_server_config( const std::string& key, const std::string& value );
	void load_links_config( const std::string& key, const std::string& value );
};
