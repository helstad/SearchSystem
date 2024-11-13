#include "text_extractor.h"

#include <algorithm>
#include <regex>
#include <sstream>

std::unordered_map<std::string, int> text_extractor::extract_and_count_words(std::string& html)
{
    GumboOutput* output = gumbo_parse(html.c_str());
    std::unordered_map<std::string, int> word_counts;

    extract_text(output->root, word_counts);

    gumbo_destroy_output(&kGumboDefaultOptions, output);
    return word_counts;
}

void text_extractor::extract_text(GumboNode* node, std::unordered_map<std::string, int>& word_count)
{
    if (node->type == GUMBO_NODE_TEXT)
    {
        const std::string str(node->v.text.text);
        process_text(str, word_count);
    }

    if (node->type == GUMBO_NODE_ELEMENT)
    {
        GumboVector* children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i)
        {
            extract_text(static_cast<GumboNode*>(children->data[i]), word_count);
        }
    }
}

void text_extractor::process_text(const std::string& text, std::unordered_map<std::string, int>& word_count)
{
    std::string lower_text = text;
    std::transform(lower_text.begin(), lower_text.end(), lower_text.begin(), ::tolower);

    const std::regex non_word("[^a-zA-Z]+");
    lower_text = std::regex_replace(lower_text, non_word, " ");

    std::istringstream stream(lower_text);
    std::string word;
    while (stream >> word)
    {
        if (word.length() >= 3 && word.length() <= 32)
        {
            word_count[word]++;
        }
        
    }
}
