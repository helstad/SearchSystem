#include "config_loader.h"

#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>

bool config_loader::load() {
  if (!std::filesystem::exists(file_name_)) {
    std::cerr << "Configuration file not found: " << file_name_ << '\n';
    return false;
  }

  std::ifstream file(file_name_);
  if (!file.is_open()) {
    std::cerr << "Failed to open configuration file: " << file_name_ << '\n';
    return false;
  }

  std::string line;
  std::string current_section;

  while (std::getline(file, line)) {
    if (line.empty() || line[0] == '#')
      continue;

    if (line[0] == '[') {
      current_section = line.substr(1, line.find(']') - 1);
      continue;
    }

    if (current_section.empty()) {
      std::cerr << "Error: No section header before key-value pair.\n";
      return false;
    }

    parse_line(line, current_section);
  }

  file.close();

  if (config_.spider.start_url.empty() && config_.links.urls.empty()) {
    std::cerr << "Error: Neither start_url nor any URLs are defined in the "
                 "configuration.\n";
  }

  return true;
}

const config &config_loader::get_config() const { return config_; }

std::vector<std::string> config_loader::get_urls() const {
  if (config_.links.urls.empty()) {
    if (!config_.spider.start_url.empty()) {
      return {config_.spider.start_url};
    } else {
      throw std::runtime_error("No one URLs in ini-file");
    }
  }
  return config_.links.urls;
}

void config_loader::parse_line(const std::string &line,
                               const std::string &section) {
  std::istringstream iss(line);
  std::string value;
  if (std::string key;
      std::getline(iss, key, '=') && std::getline(iss, value)) {
    std::erase_if(key, ::isspace);
    std::erase_if(value, ::isspace);

    if (section == "Database")
      load_database_config(key, value);
    if (section == "Spider")
      load_spider_config(key, value);
    if (section == "Server")
      load_server_config(key, value);
    if (section == "Links")
      load_links_config(key, value);
  }
}

void config_loader::load_database_config(const std::string &key,
                                         const std::string &value) {
  static const std::map<std::string, std::function<void(const std::string &)>>
      load_map = {
          {"db_host",
           [this](const std::string &v) { config_.database.host = v; }},
          {"db_port",
           [this](const std::string &v) { config_.database.port = v; }},
          {"db_name",
           [this](const std::string &v) { config_.database.dbname = v; }},
          {"db_user",
           [this](const std::string &v) { config_.database.user = v; }},
          {"db_password",
           [this](const std::string &v) { config_.database.password = v; }}};

  if (load_map.contains(key)) {
    load_map.at(key)(value);
  }
}

void config_loader::load_spider_config(const std::string &key,
                                       const std::string &value) {
  static const std::map<std::string, std::function<void(const std::string &)>>
      load_map = {
          {"start_url",
           [this](const std::string &v) { config_.spider.start_url = v; }},
          {"depth", [this](const std::string &v) {
             try {
               config_.spider.depth = std::stoi(v);
             } catch (const std::invalid_argument &) {
               std::cerr << "Invalid depth value: " << v << '\n';
             }
           }}};

  if (load_map.contains(key)) {
    load_map.at(key)(value);
  }
}

void config_loader::load_server_config(const std::string &key,
                                       const std::string &value) {
  static const std::map<std::string, std::function<void(const std::string &)>>
      load_map = {{"port", [this](const std::string &v) {
                     try {
                       config_.server.port = std::stoi(v);
                     } catch (const std::invalid_argument &) {
                       std::cerr << "Invalid port value: " << v << '\n';
                     }
                   }}};

  if (load_map.contains(key)) {
    load_map.at(key)(value);
  }
}

void config_loader::load_links_config(const std::string &key,
                                      const std::string &value) {
  if (key == "urls") {
    config_.links.urls.clear();

    if (!config_.spider.start_url.empty()) {
      config_.links.urls.push_back(config_.spider.start_url);

      if (!value.empty()) {
        std::istringstream iss(value);
        std::string url;
        while (std::getline(iss, url, ',')) {
          if (!url.empty()) {
            config_.links.urls.push_back(url);
          }
        }
      }
    }
  }
}
