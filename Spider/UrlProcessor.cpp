#include "UrlProcessor.h"
#include <regex>

UrlProcessor::UrlProcessor(DatabaseConnector& dbConnector, DatabaseManager& dbManager, int depth)
	: dbConnector_(dbConnector),
	dbManager_(dbManager),
	maxDepth_(depth),
	fetcher_(),
	textProcessor_()
{
}

void UrlProcessor::processUrl(const std::string& url, int currentDepth)
{
    if (currentDepth >= maxDepth_)
    {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(dbMutex);

        if (visitedUrls.find(url) != visitedUrls.end())
        {
            std::cout << "URL already visited: " << url << std::endl;
            return;
        }

        visitedUrls.insert(url);
    }

    try
    {
        std::string htmlContent = fetcher_.fetch(url);

        if (htmlContent.empty())
        {
            std::cerr << "Warning: Empty content for URL: " << url << std::endl;
            return;
        }

        std::map<std::string, int> wordCounts = textProcessor_.countWords(htmlContent);

        pqxx::work txn(dbConnector_.getConnection());

        try
        {
            int documentId = dbManager_.saveDocument(url, txn);

            for (const auto& pair : wordCounts)
            {
                const std::string& word = pair.first;
                int frequency = pair.second;
                int wordId = dbManager_.saveWord(word, txn);
                dbManager_.saveWordFrequency(documentId, wordId, frequency, txn);
            }

            txn.commit();
        }
        catch (const std::exception& e)
        {
            txn.abort();
            std::cerr << "Transactiob failed for URL " << url << ": " << e.what() << std::endl;
            return;
        }

        auto newUrls = extractUrls(htmlContent);
        for (const auto& newUrl : newUrls) {
            processUrl(newUrl, currentDepth + 1);
        }

        std::cout << "Fetching " << url << " completed!" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error fetching URL " << url << ": " << e.what() << std::endl;
    }
}

std::vector<std::string> UrlProcessor::extractUrls(const std::string& htmlContent)
{
    std::vector<std::string> urls;
    std::regex urlRegex(R"((http[s]?://[^\s'"]+|www\.[^\s'"]+))");
    auto urlBegin = std::sregex_iterator(htmlContent.begin(), htmlContent.end(), urlRegex);
    auto urlEnd = std::sregex_iterator();

    for (std::sregex_iterator i = urlBegin; i != urlEnd; ++i)
    {
        std::string url = i->str();
        if (visitedUrls.find(url) == visitedUrls.end())
        {
            if (isValidUrl(url))
            {
                urls.push_back(url);
            }
        }
    }

    return urls;
}

bool UrlProcessor::isValidUrl(const std::string& url)
{
    return url.find("http://") == 0 || url.find("https://") == 0;
}