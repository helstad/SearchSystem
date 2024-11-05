#pragma once

#include "ConfigLoader.h"

#include <string>
#include <pqxx/pqxx>
#include <map>

class ConfigLoader;

class DatabaseConnector
{
public:
	DatabaseConnector(const ConfigLoader& configLoader);
	~DatabaseConnector() = default;

	bool connect();
	pqxx::connection& getConnection();
	void initDatabase();

private:
	bool tableExists(pqxx::nontransaction& ntxn, const std::string& tableName);
	void createTableDocuments(pqxx::nontransaction& ntxn);
	void createTableWords(pqxx::nontransaction& ntxn);
	void createTableDocumentsWords(pqxx::nontransaction& ntxn);

	DatabaseConfig dbConfig_;
	std::unique_ptr<pqxx::connection> conn_;
};