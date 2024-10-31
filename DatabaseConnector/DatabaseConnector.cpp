#include "DatabaseConnector.h"

#include <iostream>

DatabaseConnector::DatabaseConnector(const ConfigLoader& configLoader)
    : dbConfig_(configLoader.getConfig().database), conn_(nullptr)
{
}

DatabaseConnector::~DatabaseConnector()
{
    delete conn_;
}

pqxx::connection& DatabaseConnector::getConnection()
{
    if (!conn_)
    {
        throw std::runtime_error("Database connection is not established.");
    }
    return *conn_;
}

void DatabaseConnector::initDatabase()
{
    try {
        pqxx::work txn(*conn_);

        // Создание таблиц
        txn.exec(R"(
            CREATE TABLE IF NOT EXISTS Documents (
                id SERIAL PRIMARY KEY,
                url TEXT NOT NULL UNIQUE,
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
            );
            CREATE TABLE IF NOT EXISTS Words (
                id SERIAL PRIMARY KEY,
                word TEXT UNIQUE NOT NULL
            );
            CREATE TABLE IF NOT EXISTS DocumentWordFrequency (
                document_id INT REFERENCES Documents(id) ON DELETE CASCADE,
                word_id INT REFERENCES Words(id) ON DELETE CASCADE,
                frequency INT NOT NULL,
                PRIMARY KEY (document_id, word_id)
            );
        )");

        txn.commit();
        std::cout << "Database tables initialized successfully." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error during database initialization: " << e.what() << std::endl;
        throw;
    }
}

int DatabaseConnector::saveDocument(const std::string& url)
{
    pqxx::work txn(*conn_);
    auto result = txn.exec_prepared("insert_document", url);

    if (result.empty())
    {
        throw std::runtime_error("No document was inserted, result is empty.");
    }

    int documentId = result[0][0].as<int>(); // Получаем ID вставленного документа
    txn.commit();
    return documentId;
}


int DatabaseConnector::saveWord(const std::string& word)
{
    pqxx::work txn(*conn_);
    auto result = txn.exec_prepared("insert_word", word);

    if (result.empty())
    {
        throw std::runtime_error("No word was inserted, result is empty.");
    }

    int wordId = result[0][0].as<int>(); // Получаем ID вставленного слова
    txn.commit();
    return wordId;
}


void DatabaseConnector::saveWordFrequency(int documentId, int wordId, int frequency)
{
    try {
        pqxx::work txn(*conn_);
        txn.exec_prepared("insert_word_frequency", documentId, wordId, frequency);
        txn.commit();
    }
    catch (const std::exception& e) {
        std::cerr << "Error saving word frequency: " << e.what() << std::endl;
        throw;
    }
}

bool DatabaseConnector::connect()
{
    try
    {
        conn_ = new pqxx::connection(dbConfig_.toConnectionString());
        if (conn_->is_open())
        {
            std::cout << "Connected to the database successfully." << std::endl;
            initDatabase();  // Инициализация таблиц сразу после подключения
            return true;
        }
        else
        {
            std::cerr << "Failed to connect to the database." << std::endl;
            return false;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error connecting to the database: " << e.what() << std::endl;
        return false;
    }
}

bool DatabaseConnector::prepareStatements()
{
    try {
        conn_->prepare("insert_document", "INSERT INTO Documents (url) VALUES ($1) ON CONFLICT (url) DO NOTHING RETURNING id");
        conn_->prepare("insert_word", "INSERT INTO Words (word) VALUES ($1) ON CONFLICT (word) DO NOTHING RETURNING id");
        conn_->prepare("insert_word_frequency",
            "INSERT INTO DocumentWordFrequency (document_id, word_id, frequency) "
            "VALUES ($1, $2, $3) ON CONFLICT (document_id, word_id) DO UPDATE SET frequency = EXCLUDED.frequency");

        std::cout << "Prepared statements successfully." << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error preparing statements: " << e.what() << std::endl;
        return false;
    }
}
