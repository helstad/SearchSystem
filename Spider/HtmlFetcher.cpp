#include "HtmlFetcher.h"

#include <stdexcept>
#include <iostream>

HtmlFetcher::HtmlFetcher()
{
	curl_ = curl_easy_init();
	if (!curl_)
	{
		throw std::runtime_error("Failder to init CURL.");
	}
}

HtmlFetcher::~HtmlFetcher()
{
	if (curl_)
	{
		curl_easy_cleanup(curl_);
	}
}

std::string HtmlFetcher::fetch(const std::string& url)
{
	if (!curl_)
	{
		throw std::runtime_error("CURL instance not init.");
	}

	std::string htmlContent;
	CURLcode res;

	res = curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
	if (res != CURLE_OK) {
		throw std::runtime_error("Failed to set URL option: " + std::string(curl_easy_strerror(res)));
	}

	res = curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, writeCallback);
	if (res != CURLE_OK) {
		throw std::runtime_error("Failed to set WRITEFUNCTION option: " + std::string(curl_easy_strerror(res)));
	}

	res = curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &htmlContent);
	if (res != CURLE_OK) {
		throw std::runtime_error("Failed to set WRITEDATA option: " + std::string(curl_easy_strerror(res)));
	}

	res = curl_easy_perform(curl_);
	if (res != CURLE_OK) {
		throw std::runtime_error("CURL request failed: " + std::string(curl_easy_strerror(res)));
	}

	return htmlContent;
}

size_t HtmlFetcher::writeCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

static CurlInitializer curInit;
