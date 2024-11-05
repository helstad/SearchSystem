#include "TextProcessor.h"
#include "DatabaseConnector.h"

#include <regex>
#include <sstream>
#include <cctype>
#include <algorithm>

std::map<std::string, int> TextProcessor::countWords(const std::string& htmlContent)
{
    std::string cleanedText = cleanText(htmlContent);

    std::map<std::string, int> wordCount;
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
