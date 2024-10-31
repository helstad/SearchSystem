#include "URLQueue.h"

void URLQueue::push(const std::string& url)
{
	std::lock_guard<std::mutex> lock(mutex_);
	queue_.push(url);
	cv_.notify_one();
}

bool URLQueue::pop(std::string& url)
{
	std::unique_lock<std::mutex> lock(mutex_);
	cv_.wait(lock, [this] { return !queue_.empty() || done_; });

	if (queue_.empty() && done_)
	{
		return false;
	}

	url = queue_.front();
	queue_.pop();
	return true;
}

void URLQueue::setDone()
{
	std::lock_guard<std::mutex> lock(mutex_);
	done_ = true;
	cv_.notify_all();
}

bool URLQueue::isEmpty() const
{
	std::lock_guard<std::mutex> lock(mutex_);
	return queue_.empty();
}
