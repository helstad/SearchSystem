#pragma once

#include <string>
#include <unordered_map>
#include <gumbo.h>

class text_extractor
{
public:
    text_extractor() = default;

    static std::unordered_map<std::string, int> extract_and_count_words(std::string& html);

private:
    static void extract_text(GumboNode* node, std::unordered_map<std::string, int>& word_count);
    static void process_text(const std::string& text, std::unordered_map<std::string, int>& word_count);
};

