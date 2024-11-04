#include "UrlProcessor.h"

UrlProcessor::UrlProcessor(DatabaseConnector& dbConnector)
	: dbConnector_(dbConnector),
	fetcher_(),
	textProcessor_()
{
}

void UrlProcessor::processUrl(const std::string& url)
{
	try
	{
		std::string htmlContent = fetcher_.fetch(url);
		std::map<std::string, int> wordCounts = textProcessor_.countWords(htmlContent);

		int documentId = dbConnector_.saveDocument(url);

		for (const auto& pair : wordCounts)
		{
			const std::string& word = pair.first;
			int frequency = pair.second;
			int wordId = dbConnector_.saveWord(word);
			dbConnector_.saveWordFrequency(documentId, wordId, frequency);
		}

		std::cout << "Fetching " << url << " completed!" << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error fetching URL " << url << ": " << e.what() << std::endl;
	}
}
