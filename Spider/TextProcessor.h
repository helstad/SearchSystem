#pragma once

#include <map>
#include <string>
#include <vector>

class DatabaseConnector;

class TextProcessor
{
public:
	static std::map<std::string, int> countWords(const std::string& htmlContent);

private:
	static std::string cleanText(const std::string& html);
};

