#include "url_processor.h"
#include "link_extractor.h"
#include "text_extractor.h"
#include "database_connector.h"
#include "database_operations.h"

#include <curl/curl.h>
#include <utility>
#include <regex>

size_t url_processor::write_callback(char* ptr, size_t size, size_t nmemb, void* userdata)
{
	std::string* s = static_cast<std::string*>(userdata);
	const size_t total_size = size * nmemb;
	s->append(ptr, total_size);
	return total_size;
}

std::string url_processor::fetch_html(const std::string& url)
{
	if (url.empty() || !std::regex_match(url, std::regex("https?://.*"))) {
		return "";
	}

	CURL* curl = curl_easy_init();
	if (!curl) {
		std::cerr << "Failed to init curl\n";
		return "";
	}

	std::string html_content;
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, url_processor::write_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &html_content);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

	if (const CURLcode res = curl_easy_perform(curl); res != CURLE_OK) {
		std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << ". URL: " << url << '\n';
		curl_easy_cleanup(curl);
		return "";
	}

	curl_easy_cleanup(curl);
	return html_content;
}

void url_processor::save_to_database(const std::string& url, const std::unordered_map<std::string, int>& word_count)
{
	try {
		pqxx::work txn = db_connector_.start_transaction();

		database_operations db_ops;
		int url_id = db_ops.insert_url(txn, url);
		if (url_id >= 0) {
			db_ops.insert_word_for_url(txn, url_id, word_count);
			txn.commit();
		}
		else {
			std::cerr << "Error inserting URL into database: " << url << '\n';
		}

	} catch (const std::exception& e) {
		std::cerr << "Error in save_to_database: " << e.what() << '\n';
	}
}
url_processor::url_processor(database_connector& db_connector)
	: db_connector_(db_connector)
{}

void url_processor::process_urls(const std::vector<std::string>& urls, int depth)
{
	std::unordered_set<std::string> visited_urls;
	std::queue<std::pair<std::string, int>> url_queue;

	for (const auto& url : urls) {
		url_queue.emplace(url, depth);
		visited_urls.insert(url);
	}

	while (!url_queue.empty()) {
		auto [cur_url, cur_depth] = url_queue.front();
		url_queue.pop();

		if (std::string html_content = fetch_html(cur_url); !html_content.empty()) {
			auto word_count = text_extractor::extract_and_count_words(html_content);
			save_to_database(cur_url, word_count);

			if (cur_depth > 0) {
				link_extractor extractor;
				const auto new_urls = extractor.extract_links(html_content, cur_url);

				for (const auto& new_url : new_urls) {
					if (visited_urls.contains(new_url)) continue;
					visited_urls.insert(new_url);
					url_queue.emplace(new_url, cur_depth - 1);
					std::cout << "New URL added to the queue: " << new_url << '\n';
				}
			}
		}
	}
}
