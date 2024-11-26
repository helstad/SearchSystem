#pragma once

#include <boost/algorithm/string/find_format.hpp>
#include <unordered_set>
#include <iostream>
#include <string>
#include <vector>
#include <gumbo.h>

class link_extractor {
public:
  [[nodiscard]] auto extract_links(const std::string &html_content,
                                   const std::string &base_url)
      -> std::vector<std::string>;

  std::string remove_fragment(const std::string &url);
  std::string normalize_url(const std::string &base_url,
                            const std::string &relative_url);
  std::string decode_html_entities(const std::string &url);
  bool is_valid_url(const std::string &url);
};
