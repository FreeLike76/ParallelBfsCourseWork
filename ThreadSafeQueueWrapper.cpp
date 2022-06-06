#include "ThreadSafeQueueWrapper.h"

template<class T>
T ThreadSafeQueueWrapper<T>::front()
{
	std::lock_guard<std::mutex> lock(m);
	return q.front();
}

template<class T>
void ThreadSafeQueueWrapper<T>::pop()
{
	std::lock_guard<std::mutex> lock(m);
	q.pop();
}

template<class T>
void ThreadSafeQueueWrapper<T>::push(T value)
{
	std::lock_guard<std::mutex> lock(m);
	q.push(value);
}

template<class T>
bool ThreadSafeQueueWrapper<T>::empty()
{
	std::lock_guard<std::mutex> lock(m);
	return q.empty();
}
