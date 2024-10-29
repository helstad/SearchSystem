#pragma once

#include "ConfigLoader.h"

#include <string>
#include <pqxx/pqxx>

class DatabaseConnector
{
public:
	DatabaseConnector(const ConfigLoader& configLoader);
	~DatabaseConnector();

	bool connect();
	pqxx::connection& getConnection();

private:
	DatabaseConfig dbConfig_;
	pqxx::connection* conn_;
};