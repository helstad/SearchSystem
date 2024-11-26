#include "link_extractor.h"
#include <algorithm>
#include <regex>
#include <string>
#include <unordered_set>
#include <vector>

std::vector<std::string>
link_extractor::extract_links(const std::string &html_content,
                              const std::string &base_url) {
  std::vector<std::string> links;
  std::unordered_set<std::string> unique_links;

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
    link = decode_html_entities(link);

    std::string full_url = normalize_url(base_url, link);

    if (!full_url.empty() && unique_links.insert(full_url).second) {
      if (is_valid_url(full_url)) {
        links.push_back(full_url);
      }
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

std::string link_extractor::normalize_url(const std::string &base_url,
                                          const std::string &relative_url) {
  if (relative_url.starts_with("http")) {
    return relative_url;
  }

  std::string normalized_url = relative_url;
  std::replace(normalized_url.begin(), normalized_url.end(), '\\', '/');

  if (normalized_url.starts_with("//")) {
    return "https:" + normalized_url;
  }

  if (normalized_url.starts_with("/")) {
    size_t domain_end = base_url.find('/', base_url.find("://") + 3);
    if (domain_end != std::string::npos) {
      return base_url.substr(0, domain_end) + normalized_url;
    }
    return base_url + normalized_url;
  }

  size_t last_slash_pos = base_url.rfind('/');
  if (last_slash_pos != std::string::npos) {
    return base_url.substr(0, last_slash_pos + 1) + normalized_url;
  }

  return base_url + "/" + normalized_url;
}

std::string link_extractor::decode_html_entities(const std::string &url) {
  std::string decoded_url = url;
  size_t pos = 0;
  while ((pos = decoded_url.find("&", pos)) != std::string::npos) {
    decoded_url.replace(pos, 5, "&");
    pos += 1;
  }
  return decoded_url;
}

bool link_extractor::is_valid_url(const std::string &url) {
  if (url.find("?action=") != std::string::npos ||
      url.find("?p=") != std::string::npos ||
      url.find(";a=") != std::string::npos) {
    return false;
  }

  if (url.find("(") != std::string::npos ||
      url.find(")") != std::string::npos) {
    return false;
  }

  if (url.find(":") != std::string::npos &&
      url.find("://") != std::string::npos) {
    size_t protocol_end_pos = url.find("://");
    std::string path = url.substr(protocol_end_pos + 3);
    if (path.find(":") != std::string::npos) {
      return false;
    }
  }

  static const std::unordered_set<std::string> blacklist{
      "/wiki/",     "/Category:", "/File:", "/Special:", "/User:", "_missing_",
      "index.php?", "mailto:",    ".pdf",   ".jpg",      ".png",   ".gif",
      ".css",       ".js",        ".ico",   ".svg",      ".xml",   "api.php?",
      "gitweb",     "?p=",        ";a="};

  for (const auto &prefix : blacklist) {
    if (url.find(prefix) != std::string::npos) {
      return false;
    }
  }

  return true;
}
