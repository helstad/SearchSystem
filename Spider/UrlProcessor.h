#pragma once

#include "DatabaseConnector.h"
#include "DatabaseManager.h"
#include "TextProcessor.h"
#include "HtmlFetcher.h"

#include <vector>
#include <unordered_set>
#include <mutex>

class UrlProcessor
{
public:
	explicit UrlProcessor(DatabaseConnector& dbConnector, DatabaseManager& dbManager, int depth);
	void processUrl(const std::string& url, int currentDepth = 0);
	

private:
	std::vector<std::string> extractUrls(const std::string& htmlContent);
	bool isValidUrl(const std::string& url);

	std::mutex dbMutex;
	DatabaseConnector& dbConnector_;
	DatabaseManager dbManager_;
	HtmlFetcher fetcher_;
	TextProcessor textProcessor_;
	int maxDepth_;

	std::unordered_set<std::string> visitedUrls;
};

