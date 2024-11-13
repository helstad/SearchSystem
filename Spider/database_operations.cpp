#include "database_operations.h"

int database_operations::insert_url(pqxx::work& txn, const std::string& url)
{
  try {
    const std::string check_query = "SELECT id_url FROM documents WHERE url_name = $1";

    if (const pqxx::result check_res = txn.exec_params(check_query, url); !check_res.empty()) {
      return check_res[0][0].as<int>();
    }

    const std::string insert_query = "INSERT INTO documents (url_name) VALUES ($1) ON CONFLICT (url_name) DO NOTHING RETURNING id_url";
    const pqxx::result res = txn.exec_params(insert_query, url);

    return res[0][0].as<int>();
  } catch (const std::exception& e) {
    std::cerr << "Insert error for URL: " << e.what() << '\n';
    return -1;
  }
}

int database_operations::insert_word(pqxx::work& txn, const std::string& word)
{
  try {
    const std::string check_query = "SELECT id_word FROM words WHERE word_url = $1";

    if (const pqxx::result check_res = txn.exec_params(check_query, word); !check_res.empty()) {
      return check_res[0][0].as<int>();
    }

    const std::string insert_query = "INSERT INTO words (word_url) VALUES ($1) ON CONFLICT (word_url) DO NOTHING RETURNING id_word";
    const pqxx::result res = txn.exec_params(insert_query, word);

    return res[0][0].as<int>();
  } catch (const std::exception& e) {
    std::cerr << "Insert error for word: " << e.what() << '\n';
    return -1;
  }
}

void database_operations::insert_url_word(pqxx::work& txn, int url_id, int word_id, int count)
{
  try {
    const std::string query = R"(
        INSERT INTO documents_words (url_id, word_id, freq)
        VALUES ($1, $2, $3)
        ON CONFLICT (url_id, word_id)
        DO UPDATE SET freq = documents_words.freq + EXCLUDED.freq
    )";

    txn.exec_params(query, url_id, word_id, count);
    std::cout << "Inserted or updated frequency for URL ID " << url_id << " and Word ID " << word_id << " with count " << count << '\n';
  } catch (const std::exception& e) {
    std::cerr << "Error inserting/updating word frequency: " << e.what() << '\n';
  }
}

void database_operations::insert_word_for_url(pqxx::work& txn, int url_id, const std::unordered_map<std::string, int>& word_count)
{
  try {
    for (const auto& [word, count] : word_count) {
      if (const int word_id = insert_word(txn, word); word_id != -1) {
        insert_url_word(txn, url_id, word_id, count);
      }
    }
  } catch (const std::exception& e) {
    std::cerr << "Error when linking words to a URL: " << e.what() << '\n';
  }
}
