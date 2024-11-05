#include "DatabaseConnector.h"

#include <iostream>

DatabaseConnector::DatabaseConnector(const ConfigLoader& configLoader)
	: dbConfig_(configLoader.getConfig().database)
{
}

bool DatabaseConnector::connect()
{
	try
	{
		if (!conn_)
		{
			conn_ = std::make_unique<pqxx::connection>(dbConfig_.toConnectionString());
		}


		if (conn_->is_open())
		{
			std::cout << "Connected to the database successfully." << std::endl;
			initDatabase();
			return true;
		}
		else
		{
			std::cerr << "Failed to connect to the database." << std::endl;
			return false;
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error connecting to the database: " << e.what() << std::endl;
		return false;
	}
}

pqxx::connection& DatabaseConnector::getConnection()
{
	if (!conn_)
	{
		throw std::runtime_error("Database connection is not established.");
	}
	return *conn_;
}

void DatabaseConnector::initDatabase()
{
	try {
		pqxx::nontransaction ntxn(getConnection());

		if (!tableExists(ntxn, "documents"))
		{
			createTableDocuments(ntxn);
		}

		if (!tableExists(ntxn, "words"))
		{
			createTableWords(ntxn);
		}

		if (!tableExists(ntxn, "documents_words"))
		{
			createTableDocumentsWords(ntxn);
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Error during database initialization: " << e.what() << std::endl;
		throw;
	}
}

bool DatabaseConnector::tableExists(pqxx::nontransaction& ntxn, const std::string& tableName)
{
	auto result = ntxn.exec("SELECT COUNT(*) FROM information_schema.tables WHERE table_name = '" + tableName + "'");
	return result[0][0].as<int>() > 0;
}

void DatabaseConnector::createTableDocuments(pqxx::nontransaction& ntxn)
{
	ntxn.exec(R"(
		CREATE TABLE IF NOT EXISTS documents(
			id SERIAL PRIMARY KEY,
			url VARCHAR(2500) NOT NULL UNIQUE
		);
	)");
}

void DatabaseConnector::createTableWords(pqxx::nontransaction& ntxn)
{
	ntxn.exec(R"(
		CREATE TABLE IF NOT EXISTS words(
			id SERIAL PRIMARY KEY,
			word VARCHAR(32) NOT NULL UNIQUE
		);
	)");
}

void DatabaseConnector::createTableDocumentsWords(pqxx::nontransaction& ntxn)
{
	ntxn.exec(R"(
		CREATE TABLE IF NOT EXISTS documents_words(
			document_id INTEGER REFERENCES documents(id),
			word_id INTEGER REFERENCES words(id),
			frequency INTEGER NOT NULL,
			CONSTRAINT pk PRIMARY KEY(document_id, word_id)
		);
	)");
}
