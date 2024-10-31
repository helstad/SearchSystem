#pragma once

#include <map>
#include <string>
#include <vector>

class DatabaseConnector;

class TextProcessor
{
public:
	static std::map<std::string, int> countWords(const std::string& htmlContent);
	static void saveWordFrequencies(DatabaseConnector& dbConnector, const std::string& url, const std::map<std::string, int>& wordCounts);

private:
	static std::string cleanText(const std::string& html);
};

