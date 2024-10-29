#include "DatabaseConnector.h"

#include <iostream>

DatabaseConnector::DatabaseConnector(const ConfigLoader& configLoader)
	: dbConfig_(configLoader.getConfig().database), conn_(nullptr)
{
}

DatabaseConnector::~DatabaseConnector()
{
	delete conn_;
}

pqxx::connection& DatabaseConnector::getConnection()
{
	if (!conn_)
	{
		throw std::runtime_error("Database connection is not established.");
	}
	return *conn_;
}

bool DatabaseConnector::connect()
{
	try
	{
		conn_ = new pqxx::connection(dbConfig_.toConnectionString());
		if (conn_->is_open())
		{
			std::cout << "Connected to the database successfuly." << std::endl;
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
		std::cerr << "Error connectiong to the database: " << e.what() << std::endl;
		return false;
	}
}
