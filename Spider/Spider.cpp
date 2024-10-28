// Spider.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "ConfigManager.h"

#include <iostream>

int main()
{
    setlocale(LC_ALL, "Russian");

    ConfigManager config("config.ini");

    if (!config.load())
    {
        return 1;
    }

    std::string url = config.get("url");
    std::string dbHost = config.get("db_host");
    std::string dbUser = config.get("db_user");
    std::string dbPassword = config.get("db_password");
    std::string dbName = config.get("db_name");

    std::cout << "URL: " << url << std::endl;
    std::cout << "Database Host: " << dbHost << std::endl;
    std::cout << "Database User: " << dbUser << std::endl;
    std::cout << "Database Password: " << dbPassword << std::endl;
    std::cout << "Database Name: " << dbName << std::endl;

    return 0;
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
