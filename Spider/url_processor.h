#pragma once

#include <curl/curl.h>

#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "database_connector.h"

class url_processor {
public:
  explicit url_processor(database_connector &db_connector);
  void process_urls_multithreaded(const std::vector<std::string> &urls,
                                  int depth, int thread_count);

private:
  void process_urls(const std::vector<std::string> &urls, int depth);
  std::string fetch_html(const std::string &url);
  void save_to_database(const std::string &url,
                        const std::unordered_map<std::string, int> &word_count);
  static size_t write_callback(char *ptr, size_t size, size_t nmemb,
                               void *userdata);

  database_connector *db_connector_;
};
