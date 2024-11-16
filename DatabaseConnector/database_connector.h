#pragma once

#include "config_loader.h"
#include <memory>
#include <pqxx/pqxx>

class config_loader;

class database_connector {
public:
  explicit database_connector(const config_loader &config_loader);
  ~database_connector() = default;

  database_connector(const database_connector &) = delete;
  database_connector &operator=(const database_connector &) = delete;
  database_connector(database_connector &&) = delete;
  database_connector &operator=(database_connector &&) = delete;

  [[nodiscard]] auto is_connected() const -> bool;
  [[nodiscard]] auto create_connection() const
      -> std::shared_ptr<pqxx::connection>;
  [[nodiscard]] auto
  start_transaction(const std::shared_ptr<pqxx::connection> &conn) const
      -> pqxx::work;
  void init_database();

private:
  bool table_exists(pqxx::work &txn, const std::string &table_name);
  void create_table_documents(pqxx::work &txn);
  void create_table_words(pqxx::work &txn);
  void create_table_documents_words(pqxx::work &txn);

  database_config db_config_;
  std::shared_ptr<pqxx::connection> conn_;
};
