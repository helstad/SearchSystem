#pragma once

#include "DatabaseConnector.h"
#include "TextProcessor.h"
#include "HtmlFetcher.h"

class UrlProcessor
{
public:
	explicit UrlProcessor(DatabaseConnector& dbConnector);
	void processUrl(const std::string& url);

private:
	DatabaseConnector& dbConnector_;
	HtmlFetcher fetcher_;
	TextProcessor textProcessor_;
};

