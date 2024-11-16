#pragma once

#include <boost/beast/http.hpp>
#include <memory>
#include <pqxx/pqxx>

namespace beast = boost::beast;
namespace http = beast::http;

class request_handler {
public:
  explicit request_handler(std::shared_ptr<pqxx::connection> db_conn);

  void handle_request(const http::request<http::string_body> &req,
                      http::response<http::string_body> &res) const;
  std::string generate_result_template(const std::string &query) const;

private:
  std::shared_ptr<pqxx::connection> db_conn_;
};