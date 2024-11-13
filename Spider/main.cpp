#include "config_loader.h"
#include "url_processor.h"
#include "database_connector.h"

#include <Windows.h>
#include <iostream>
#include <thread>
#include <vector>


int main()
{
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);

	std::cout << "Spider process start.\n";

	config_loader config_loader;
	if (!config_loader.load()) { return -1; }

	database_connector db_connector(config_loader);
	if (!db_connector.is_connected()) { return -1; }
	
	const auto& urls = config_loader.get_urls();

	for (const auto& url : urls) {
		std::cout << "Set URL: " << url << '\n';
	}

	url_processor url_process(db_connector);
	url_process.process_urls(urls, 1);

	std::cout << "\r\nSpider process end.\n";
	
	return 0;
}
