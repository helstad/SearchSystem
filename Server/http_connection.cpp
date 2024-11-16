#include "http_connection.h"

#include <codecvt>
#include <iomanip>
#include <iostream>
#include <locale>
#include <pqxx/connection.hxx>
#include <sstream>

#include "config_loader.h"
#include "database_connector.h"
#include "request_handler.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

std::string url_decode(const std::string &encoded) {
  std::string res;
  std::istringstream iss(encoded);
  char ch;

  while (iss.get(ch)) {
    if (ch == '%') {
      int hex;
      iss >> std::hex >> hex;
      res += static_cast<char>(hex);
    } else {
      res += ch;
    }
  }

  return res;
}

std::string convert_to_utf8(const std::string &str) {
  std::string url_decoded = url_decode(str);
  return url_decoded;
}

http_connection::http_connection(tcp::socket socket,
                                 std::shared_ptr<pqxx::connection> db_conn)
    : socket_(std::move(socket)), db_conn_(std::move(db_conn)) {}

void http_connection::start() {
  read_request();
  check_deadline();
}

void http_connection::read_request() {
  auto self = shared_from_this();

  http::async_read(socket_, buffer_, request_,
                   [self](beast::error_code ec, std::size_t bytes_transferred) {
                     boost::ignore_unused(bytes_transferred);
                     if (!ec)
                       self->process_request();
                   });
}

void http_connection::process_request() {
  response_.version(request_.version());
  response_.keep_alive(false);

  switch (request_.method()) {
  case http::verb::get:
    response_.result(http::status::ok);
    response_.set(http::field::server, "Beast");
    create_response_get();
    break;
  case http::verb::post:
    response_.result(http::status::ok);
    response_.set(http::field::server, "Beast");
    create_response_post();
    break;

  default:
    response_.result(http::status::bad_request);
    response_.set(http::field::content_type, "text/plain");
    beast::ostream(response_.body())
        << "Invalid request-method '" << std::string(request_.method_string())
        << "'";
    break;
  }

  write_response();
}

void http_connection::create_response_get() {
  if (request_.target() == "/") {
    response_.set(http::field::content_type, "text/html");
    beast::ostream(response_.body())
        << "<html>\n"
        << "<head><meta charset=\"UTF-8\"><title>Search Engine</title></head>\n"
        << "<body>\n"
        << "<h1>Search Engine</h1>\n"
        << "<p>Welcome!<p>\n"
        << "<form action=\"/\" method=\"post\">\n"
        << "    <label for=\"search\">Search:</label><br>\n"
        << "    <input type=\"text\" id=\"search\" name=\"search\"><br>\n"
        << "    <input type=\"submit\" value=\"Search\">\n"
        << "</form>\n"
        << "</body>\n"
        << "</html>\n";
  } else {
    response_.result(http::status::not_found);
    response_.set(http::field::content_type, "text/plain");
    beast::ostream(response_.body()) << "File not found\r\n";
  }
}

void http_connection::create_response_post() {
  if (request_.target() == "/") {
    std::string s = buffers_to_string(request_.body().data());

    std::cout << "POST data: " << s << '\n';

    size_t pos = s.find('=');
    if (pos == std::string::npos) {
      response_.result(http::status::not_found);
      response_.set(http::field::content_type, "text/plain");
      beast::ostream(response_.body()) << "File not found\r\n";
      return;
    }

    std::string key = s.substr(0, pos);
    std::string value = s.substr(pos + 1);

    std::string utf8value = convert_to_utf8(value);

    if (key != "search") {
      response_.result(http::status::not_found);
      response_.set(http::field::content_type, "text/plain");
      beast::ostream(response_.body()) << "File not found\r\n";
      return;
    }

    request_handler handler(db_conn_);
    std::string result_html = handler.generate_result_template(utf8value);

    response_.set(http::field::content_type, "text/html");
    beast::ostream(response_.body())
        << "<html>\n"
        << "<head><meta charset=\"UTF-8\"><title>Search Engine</title></head>\n"
        << "<body>\n"
        << "<h1>Search Engine</h1>\n"
        << "<p>Response:<p>\n"
        << result_html << "</body>\n"
        << "</html>\n";
  }
}

void http_connection::write_response() {
  auto self = shared_from_this();

  response_.content_length(response_.body().size());

  http::async_write(socket_, response_,
                    [self](beast::error_code ec, std::size_t) {
                      self->socket_.shutdown(tcp::socket::shutdown_send, ec);
                      self->deadline_.cancel();
                    });
}

void http_connection::check_deadline() {
  auto self = shared_from_this();

  deadline_.async_wait([self](beast::error_code ec) {
    if (!ec) {
      self->socket_.close(ec);
    }
  });
}
