#pragma once

#include <iostream>
#include <mutex>
#include <string>
#include <unordered_set>
#include <vector>
#include <queue>
#include <unordered_map>

#include "database_connector.h"
#include <curl/curl.h>

class url_processor
{
public:
    explicit url_processor(database_connector& db_connector);

    void process_urls(const std::vector<std::string>& urls, int depth);

private:
    std::string fetch_html(const std::string& url);
    void save_to_database(const std::string& url, const std::unordered_map<std::string, int>& word_count);
    static size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata);

    database_connector& db_connector_;
};
