#include "database_connector.h"
#include <iostream>

database_connector::database_connector(const config_loader &config_loader)
    : db_config_(config_loader.get_config().database) {
  try {
    conn_ =
        std::make_shared<pqxx::connection>(db_config_.to_connection_string());
    if (conn_ && conn_->is_open()) {
      std::cout << "Connected to the database successfully.\n";
      init_database();
    } else {
      std::cerr << "Failed to connect to database.\n";
    }
  } catch (const std::exception &e) {
    std::cerr << "Error connecting to the database: " << e.what() << '\n';
    throw;
  }
}

bool database_connector::is_connected() const {
  return conn_ && conn_->is_open();
}

std::shared_ptr<pqxx::connection>
database_connector::create_connection() const {
  try {
    auto conn =
        std::make_shared<pqxx::connection>(db_config_.to_connection_string());
    if (!conn->is_open()) {
      std::cerr << "Failed to open database connection.\n";
      return nullptr;
    }
    return conn;
  } catch (const std::exception &e) {
    std::cerr << "Error creating connection: " << e.what() << '\n';
    return nullptr;
  }
}

pqxx::work database_connector::start_transaction(
    const std::shared_ptr<pqxx::connection> &conn) const {
  try {
    return pqxx::work(*conn);
  } catch (const pqxx::sql_error &e) {
    std::cerr << "SQL error during transaction start: " << e.what()
              << "\nQuery: " << e.query() << '\n';
    throw;
  } catch (const std::exception &e) {
    std::cerr << "Error starting transaction: " << e.what() << '\n';
    throw;
  }
}

void database_connector::init_database() {
  try {
    pqxx::work txn(*conn_);
    if (!table_exists(txn, "documents")) {
      create_table_documents(txn);
    }
    if (!table_exists(txn, "words")) {
      create_table_words(txn);
    }
    if (!table_exists(txn, "documents_words")) {
      create_table_documents_words(txn);
    }
    txn.commit();
  } catch (const pqxx::sql_error &e) {
    std::cerr << "SQL error during database initialization: " << e.what()
              << "\nQuery: " << e.query() << '\n';
    throw;
  } catch (const std::exception &e) {
    std::cerr << "Error during database initialization: " << e.what() << '\n';
    throw;
  }
}

bool database_connector::table_exists(pqxx::work &txn,
                                      const std::string &table_name) {
  const auto result = txn.exec(
      "SELECT COUNT(*) FROM information_schema.tables WHERE table_name = '" +
      table_name + "'");
  return result[0][0].as<int>() > 0;
}

void database_connector::create_table_documents(pqxx::work &txn) {
  txn.exec(R"(  
		CREATE TABLE IF NOT EXISTS documents (
			id_url serial PRIMARY KEY,
			url_name VARCHAR(2048) NOT NULL,
			CONSTRAINT url_unique UNIQUE(url_name)
		);
	)");
  txn.exec("CREATE INDEX IF NOT EXISTS idx_documents_url_name ON "
           "documents(url_name);");
}

void database_connector::create_table_words(pqxx::work &txn) {
  txn.exec(R"(
		CREATE TABLE IF NOT EXISTS words (
			id_word serial PRIMARY KEY,
			word_url VARCHAR(32) NOT NULL,
			CONSTRAINT word_unique UNIQUE(word_url)
		);
	)");
  txn.exec("CREATE INDEX IF NOT EXISTS idx_words_word_url ON words(word_url);");
}

void database_connector::create_table_documents_words(pqxx::work &txn) {
  txn.exec(R"(
		CREATE TABLE IF NOT EXISTS documents_words (
			id_freq serial PRIMARY KEY,
			word_id INT NOT NULL,
			url_id INT NOT NULL,
			freq INT NOT NULL,
			CONSTRAINT documents_words_unique UNIQUE(word_id, url_id),
			CONSTRAINT documents_words_url_fk FOREIGN KEY(url_id) REFERENCES documents(id_url),
			CONSTRAINT documents_words_word_fk FOREIGN KEY(word_id) REFERENCES words(id_word)
		);
	)");
  txn.exec("CREATE INDEX IF NOT EXISTS idx_documents_words_url_id ON "
           "documents_words(url_id);");
  txn.exec("CREATE INDEX IF NOT EXISTS idx_documents_words_word_id ON "
           "documents_words(word_id);");
}
