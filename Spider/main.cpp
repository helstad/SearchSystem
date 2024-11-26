#include "config_loader.h"
#include "database_connector.h"
#include "url_processor.h"

#include <Windows.h>
#include <iostream>
#include <thread>
#include <vector>

int main() {
  SetConsoleCP(CP_UTF8);
  SetConsoleOutputCP(CP_UTF8);

  std::cout << "Spider process start.\n";

  config_loader config_loader;
  if (!config_loader.load()) {
    return -1;
  }

  for (auto &url : config_loader.get_urls()) {
    std::cout << "URL: " << url << '\n';
  }

  database_connector db_connector(config_loader);
  if (!db_connector.is_connected()) {
    return -1;
  }

  const auto &urls = config_loader.get_urls();
  const int depth = config_loader.get_config().spider.depth;
  const int thread_count = std::thread::hardware_concurrency();

  url_processor url_process(db_connector);
  url_process.process_urls_multithreaded(urls, depth, thread_count);

  std::cout << "\r\nSpider process end.\n";

  return 0;
}
