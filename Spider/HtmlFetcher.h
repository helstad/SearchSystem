#pragma once

#include <string>
#include <curl/curl.h>

struct CurlInitializer
{
	CurlInitializer()
	{
		curl_global_init(CURL_GLOBAL_DEFAULT);
	}
	~CurlInitializer()
	{
		curl_global_cleanup();
	}
};

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

