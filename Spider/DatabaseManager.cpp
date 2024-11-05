#include "DatabaseManager.h"

DatabaseManager::DatabaseManager(DatabaseConnector& dbConnector)
	: dbConnector_(dbConnector)
{
}

bool DatabaseManager::prepareStatements()
{
    try {
        dbConnector_.getConnection().prepare("insert_document",
            "INSERT INTO documents (url) VALUES ($1) "
            "ON CONFLICT (url) DO UPDATE SET url = EXCLUDED.url RETURNING id;");


        dbConnector_.getConnection().prepare("insert_word",
            "INSERT INTO words (word) VALUES ($1) "
            "ON CONFLICT (word) DO UPDATE SET word = EXCLUDED.word RETURNING id;");

        dbConnector_.getConnection().prepare("insert_word_frequency",
            "INSERT INTO documents_words (document_id, word_id, frequency) "
            "VALUES ($1, $2, $3) ON CONFLICT (document_id, word_id) DO UPDATE SET frequency = EXCLUDED.frequency;");

        std::cout << "Prepared statements successfully." << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error preparing statements: " << e.what() << std::endl;
        return false;
    }
}

int DatabaseManager::saveDocument(const std::string& url, pqxx::work& txn) {
    auto result = txn.exec_prepared("insert_document", url);
    if (result.empty()) {
        throw std::runtime_error("No document was inserted, result is empty.");
    }

    int documentId = result[0][0].as<int>();
    //txn.commit();
    return documentId;
}

int DatabaseManager::saveWord(const std::string& word, pqxx::work& txn) {
    auto result = txn.exec_prepared("insert_word", word);
    if (result.empty()) {
        throw std::runtime_error("No word was inserted, result is empty.");
    }

    int wordId = result[0][0].as<int>();
    //txn.commit();
    return wordId;
}

void DatabaseManager::saveWordFrequency(int documentId, int wordId, int frequency, pqxx::work& txn) {
    txn.exec_prepared("insert_word_frequency", documentId, wordId, frequency);
    //txn.commit();
}

void DatabaseManager::recordWordFrequencies(int documentId, const std::map<std::string, int>& wordCounts, pqxx::work& txn) {
    for (const auto& [word, count] : wordCounts) {
        int wordId = saveWord(word, txn);
        saveWordFrequency(documentId, wordId, count, txn);
    }
}

