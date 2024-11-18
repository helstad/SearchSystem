#include "config_loader.h"
#include "database_connector.h"
#include "http_connection.h"
#include <Windows.h>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>

void httpServer(tcp::acceptor &acceptor, tcp::socket &socket,
                database_connector &db_connector) {
  acceptor.async_accept(socket, [&](beast::error_code ec) {
    if (!ec) {
      auto db_conn = db_connector.create_connection();
      if (!db_conn) {
        std::cerr << "Failed to create database connection for client.\n";
      } else {
        std::make_shared<http_connection>(std::move(socket), db_conn)->start();
      }
    }
    httpServer(acceptor, socket, db_connector);
  });
}

int main() {
  SetConsoleCP(CP_UTF8);
  SetConsoleOutputCP(CP_UTF8);

  try {
    config_loader config;
    if (!config.load()) {
      return -1;
    }
    std::cout << "Connection string: "
              << config.get_config().database.to_connection_string() << '\n';

    database_connector db_connector(config);

    if (!db_connector.is_connected()) {
      std::cout << "Connection string: "
                << config.get_config().database.to_connection_string() << '\n';
      std::cerr << "Database connection failed.\n";
      return EXIT_FAILURE;
    }

    auto const address = net::ip::make_address("0.0.0.0");
    unsigned short port = config.get_config().server.port;

    net::io_context ioc{1};

    tcp::acceptor acceptor{ioc, {address, port}};
    tcp::socket socket{ioc};

    httpServer(acceptor, socket, db_connector);

    std::cout << "Open browser and connect to http://localhost:" << port << " to see the "
                 "web server operating"
              << '\n';

    ioc.run();
  } catch (std::exception const &e) {
    std::cerr << "Error: " << e.what() << '\n';
    return EXIT_FAILURE;
  }
}
