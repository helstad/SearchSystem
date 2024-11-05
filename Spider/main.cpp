#include "ConfigLoader.h"
#include "DatabaseConnector.h"
#include "UrlQueue.h"
#include "UrlProcessor.h"
#include "TextProcessor.h"

#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>

static void fetchURL(UrlQueue& queue, DatabaseConnector& dbConnector, int threadId)
{
    UrlProcessor processor(dbConnector);

    std::string url;
    while (queue.pop(url))
    {
        std::cout << "Thread " << threadId << " starting fetch for: " << url << std::endl;
        processor.processUrl(url);
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

    UrlQueue queue;

    const auto& urls = configLoader.getConfig().links.urls;
    if (urls.empty())
    {
        std::cout << "Warning: No URLs in the [Links] section. Using default start URL." << std::endl;
        queue.push(configLoader.getConfig().spider.start_url);
    }
    else
    {
        for (const auto& url : urls)
        {
            queue.push(url);
        }
    }

    std::cout << "Initialization complete, start fetching..." << std::endl;

    const int numThreads = (std::max)(std::thread::hardware_concurrency(), 1u);
    std::cout << "Defined threads count: " << numThreads << std::endl;

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