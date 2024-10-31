#pragma once

#include <string>
#include <curl/curl.h>

class HtmlFetcher
{
public:
	HtmlFetcher();
	~HtmlFetcher();

	std::string fetch(const std::string& url);

private:
	CURL* curl_;

	static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp);
};

