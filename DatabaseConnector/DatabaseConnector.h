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
	~DatabaseConnector();

	bool connect();
	bool prepareStatements();
	pqxx::connection& getConnection();
	void initDatabase();
	int saveDocument(const std::string& url);
	int saveWord(const std::string& word);
	void saveWordFrequency(int documentId, int wordId, int frequency);

private:
	DatabaseConfig dbConfig_;
	pqxx::connection* conn_;
};