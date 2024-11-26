#include "link_extractor.h"
#include <regex>
#include <vector>
#include <string>

std::vector<std::string>
link_extractor::extract_links(const std::string &html_content,
                              const std::string &base_url) {
  std::vector<std::string> links;
  std::regex pattern("<a\\s+(?:[^>]*?\\s+)?href=\"([^\"]*)\"");

  auto links_begin =
      std::sregex_iterator(html_content.begin(), html_content.end(), pattern);
  auto links_end = std::sregex_iterator();

  for (std::sregex_iterator i = links_begin; i != links_end; ++i) {
    std::string link = (*i)[1].str();

    if (link.starts_with("#")) {
      continue;
    }
    
    link = remove_fragment(link);

    if (link.starts_with("http")) {
      links.push_back(link);
    } else if (link.starts_with("/")) {
      std::string full_url = base_url + link;
      links.push_back(full_url);
    }
  }

  return links;
}

std::string link_extractor::remove_fragment(const std::string &url) {
  size_t pos = url.find('#');
  if (pos != std::string::npos) {
    return url.substr(0, pos);
  }

  return url;
}
