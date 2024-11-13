#pragma once

#include <boost/algorithm/string/find_format.hpp>
#include <string>
#include <iostream>
#include <vector>

class link_extractor
{
public:
	[[nodiscard]] auto extract_links(const std::string& html_content, const std::string& base_url) -> std::vector<std::string>;
};

