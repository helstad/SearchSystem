#pragma once

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <pqxx/pqxx>

#include "request_handler.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

class http_connection : public std::enable_shared_from_this<http_connection> {
protected:
  tcp::socket socket_;
  beast::flat_buffer buffer_{8192};
  http::request<http::dynamic_body> request_;
  http::response<http::dynamic_body> response_;
  std::shared_ptr<pqxx::connection> db_conn_;
  net::steady_timer deadline_{socket_.get_executor(), std::chrono::seconds(60)};

  void read_request();
  void process_request();
  void create_response_get();
  void create_response_post();
  void write_response();
  void check_deadline();

public:
  http_connection(tcp::socket socket,
                  std::shared_ptr<pqxx::connection> db_conn);
  void start();
};
