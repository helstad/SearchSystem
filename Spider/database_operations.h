#pragma once

#include "database_connector.h"

class database_operations {
public:
  explicit database_operations() = default;

  int insert_url(pqxx::work &txn, const std::string &url);
  int insert_word(pqxx::work &txn, const std::string &word);
  void insert_url_word(pqxx::work &txn, int url_id, int word_id, int count);
  void
  insert_word_for_url(pqxx::work &txn, int url_id,
                      const std::unordered_map<std::string, int> &word_count);
};