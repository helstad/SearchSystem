//#include "ConfigLoader.h"
//
//int main()
//{
//	ConfigLoader configLoader;
//
//	if (configLoader.load())
//	{
//		DatabaseConfig dbConfig = configLoader.getDatabaseConfig();
//		std::cout << "Connection String: " << dbConfig.toConnectionString() << std::endl;
//	}
//
//	return 0;
//}

#include "ConfigLoader.h"
#include "DatabaseConnector.h"

#include <iostream>

int main()
{
    std::cout << "Start!" << std::endl;

    ConfigLoader configLoader;
    if (!configLoader.load())
    {
        return -1;
    }

    DatabaseConnector dbConnector(configLoader);
    if (!dbConnector.connect())
    {
        return -1;
    }

    try {
        pqxx::work W(dbConnector.getConnection()); // Получаем объект для работы с транзакцией

        // Создаем таблицу
        W.exec("CREATE TABLE IF NOT EXISTS test_table (id SERIAL PRIMARY KEY, name VARCHAR(100));");
        std::cout << "Table created successfully." << std::endl;

        // Вставляем строку
        W.exec("INSERT INTO test_table (name) VALUES ('Test Name');");
        W.commit(); // Фиксируем изменения
        std::cout << "Row inserted successfully." << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    std::cout << "Program finish!" << std::endl;
    //ConfigLoader configLoader;

    //if (configLoader.load())
    //{
    //    const Config& config = configLoader.getConfig();
    //    std::cout << "Database Connection String: " << config.database.toConnectionString() << std::endl;
    //    std::cout << "Spider Start URL: " << config.spider.start_url << " Depth: " << config.spider.depth << std::endl;
    //    std::cout << "Server Port: " << config.server.port << std::endl;
    //}

    return 0;
}