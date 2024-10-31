#include "ConfigLoader.h"
#include "DatabaseConnector.h"
#include "URLQueue.h"
#include "HtmlFetcher.h"
#include "TextProcessor.h"

#include <iostream>
#include <thread>
#include <vector>

static void fetchURL(URLQueue& queue, DatabaseConnector& dbConnector, int threadId)
{
    HtmlFetcher fetcher;

    std::string url;
    while (queue.pop(url))
    {
        std::cout << "Thread " << threadId << " starting fetch for: " << url << std::endl;
        try
        {
            std::string htmlContent = fetcher.fetch(url);
            std::cout << "Thread " << threadId << " fetched content from: " << url
                << " [Length: " << htmlContent.size() << " characters]\n";
          
            std::map<std::string, int> wordCounts = TextProcessor::countWords(htmlContent);

            int documentId = dbConnector.saveDocument(url);

            for (const auto& pair : wordCounts)
            {
                const std::string& word = pair.first;
                int frequency = pair.second;
                int wordId = dbConnector.saveWord(word);
                dbConnector.saveWordFrequency(documentId, wordId, frequency);
            }
            //std::cout << "Word counts:" << std::endl;
            //for (const auto& pair : wordCounts)
            //{
            //    std::cout << pair.first << ": " << pair.second << " count" << std::endl;
            //}

            std::cout << "Fetching " << url << " completed!" << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cerr << "Thread " << threadId << " error fetching URL " << url << ": " << e.what() << "\n";
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

int main()
{
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    std::cout << "Start!" << std::endl;

    ConfigLoader configLoader;
    if (!configLoader.load())
    {
        return -1;
    }

    DatabaseConnector dbConnector(configLoader);
    if (!dbConnector.connect())
    {
        return -1;
    }

    dbConnector.initDatabase();
    std::cout << "Tables create or exist." << std::endl;

    if (!dbConnector.prepareStatements())
    {
        std::cerr << "Error preparing statements." << std::endl;
        return -1;
    }

    URLQueue queue;

    queue.push("https://ya.ru/");
    queue.push("https://shazoo.ru/");
    queue.push("https://dtf.ru/");


    std::cout << "Initialization complete, starting main processing loop..." << std::endl;

    const int numThreads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;

    for (int i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(fetchURL, std::ref(queue), std::ref(dbConnector), i + 1);
    }

    queue.setDone();

    for (auto& thread : threads)
    {
        thread.join();
    }

    std::cout << "Program finish!" << std::endl;

    return 0;
}