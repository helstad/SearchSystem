#pragma once

#include <queue>
#include <string>
#include <mutex>
#include <condition_variable>

class UrlQueue
{
public:
	void push(const std::string& url);
	bool pop(std::string& url);
	void setDone();
	bool isEmpty() const;

private:
	mutable std::mutex mutex_;
	std::condition_variable cv_;
	std::queue<std::string> queue_;
	bool done_ = false;
};

