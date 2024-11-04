#include "TextProcessor.h"
#include "DatabaseConnector.h"

#include <regex>
#include <sstream>
#include <cctype>
#include <algorithm>

std::map<std::string, int> TextProcessor::countWords(const std::string& htmlContent)
{
    std::map<std::string, int> wordCount;

    std::string cleanedText = cleanText(htmlContent);

    std::istringstream iss(cleanedText);
    std::string word;
    while (iss >> word) {
        std::transform(word.begin(), word.end(), word.begin(), ::tolower);
        
        if (word.length() >= 3 && word.length() <= 32)
        {
            wordCount[word]++;
        }
    }

    return wordCount;
}

void TextProcessor::saveWordFrequencies(DatabaseConnector& dbConnector, const std::string& url, const std::map<std::string, int>& wordCounts)
{
    try
    {
        int documentId = dbConnector.saveDocument(url);

        for (const auto& [word, count] : wordCounts)
        {
            int wordId = dbConnector.saveWord(word);
            dbConnector.saveWordFrequency(documentId, wordId, count);
        }

        std::cout << "Save data for URL " << url << " completed." << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error save data for URL " << url << ": " << e.what() << std::endl;
    }
}

std::string TextProcessor::cleanText(const std::string& html)
{
    std::string cleanedHtml = html;

    cleanedHtml = std::regex_replace(cleanedHtml, std::regex("<[^>]*>"), " ");
    cleanedHtml = std::regex_replace(cleanedHtml, std::regex("[^a-zA-Z0-9 ]"), " ");
    cleanedHtml = std::regex_replace(cleanedHtml, std::regex("\\b\\w{1,2}\\b"), " ");
    cleanedHtml = std::regex_replace(cleanedHtml, std::regex("\\b\\w*\\d+\\w*\\b"), " ");

    std::transform(cleanedHtml.begin(), cleanedHtml.end(), cleanedHtml.begin(), ::tolower);

    return cleanedHtml;
}
