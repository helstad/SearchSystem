#pragma once

#include <boost/algorithm/string/find_format.hpp>
#include <gumbo.h>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

class link_extractor {
public:
	[[nodiscard]] auto
		extract_links( const std::string& html_content,
			const std::string& base_url ) -> std::vector<std::string>;

	std::string remove_fragment( const std::string& url );
	std::string normalize_url( const std::string& base_url,
		const std::string& relative_url );
	std::string normalize_absolute_url( const std::string& url );
	bool is_valid_url( const std::string& url );
};
