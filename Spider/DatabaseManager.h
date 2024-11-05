#pragma once

#include "DatabaseConnector.h"
#include <map>
#include <string>
#include <pqxx/pqxx>


class DatabaseManager
{
public:
	DatabaseManager(DatabaseConnector& dbConnector);

	bool prepareStatements();

	int saveDocument(const std::string& url, pqxx::work& txn);
	int saveWord(const std::string& word, pqxx::work& txn);
	void saveWordFrequency(int documentId, int wordId, int frequency, pqxx::work& txn);
	void recordWordFrequencies(int documentId, const std::map<std::string, int>& wordCounts, pqxx::work& txn);

private:
	DatabaseConnector& dbConnector_;
};

