#include "request_handler.h"
#include <algorithm>
#include <boost/beast/http.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

request_handler::request_handler(std::shared_ptr<pqxx::connection> db_conn)
    : db_conn_(db_conn) {}

void request_handler::handle_request(
    const http::request<http::string_body> &req,
    http::response<http::string_body> &res) const {
  if (!db_conn_) {
    std::cerr << "Database connection is null!\n";
    res.result(http::status::internal_server_error);
    res.body() = "Internal Server Error";
    return;
  }
}

std::string
request_handler::generate_result_template(const std::string &query) const {
  // Очищаем запрос от ненужных символов, например заменяем "+" на пробелы
  std::string clean_query = query;
  std::ranges::replace(clean_query, '+', ' ');

  // Разбиваем запрос на отдельные слова
  std::istringstream query_stream(clean_query);
  std::vector<std::string> words;
  std::string word;
  while (query_stream >> word) {
    words.push_back(word);
  }

  std::for_each(words.begin(), words.end(), [](auto &word) {
    std::ranges::transform(word, word.begin(), ::tolower);
                });

  pqxx::work txn(*db_conn_);

  std::string words_placeholders;
  for (size_t i = 0; i < words.size(); ++i) {
    if (i > 0)
      words_placeholders += ", ";
    words_placeholders += txn.quote(words[i]);
  }

  const std::string search_query =
      "SELECT d.url_name, SUM(dw.freq) AS total_freq "
      "FROM documents d "
      "JOIN documents_words dw ON d.id_url = dw.url_id "
      "JOIN words w ON dw.word_id = w.id_word "
      "WHERE LOWER(w.word_url) IN (" +
      words_placeholders +
      ") "
      "GROUP BY d.id_url "
      "ORDER BY total_freq DESC "
      "LIMIT 10;";

  const pqxx::result r = txn.exec(search_query);

  std::stringstream results_html;
  if (r.empty()) {
    results_html << "<p>No results found for query: " << query << "</p>";
  } else {
    for (const auto &row : r) {
      const std::string url = row["url_name"].c_str();
      const int total_frequency = row["total_freq"].as<int>();

      results_html << "<li><a href=\"" << url << "\">" << url << "</a>"
                   << "<p><strong>Total Frequency: " << total_frequency
                   << "</strong></p></li>";
    }
  }

  return results_html.str();
}
