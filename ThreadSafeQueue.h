#pragma once

#include "stdafx.h"

template <class T>
class ThreadSafeQueue
{
private:
	std::queue<T> q;
	mutable std::mutex m;
public:
	void push(T value);
	bool empty();
	T front();
	void pop();
};

template<class T>
inline void ThreadSafeQueue<T>::push(T value)
{
	std::lock_guard<std::mutex> lock(m);
	q.push(value);
}

template<class T>
inline bool ThreadSafeQueue<T>::empty()
{
	std::lock_guard<std::mutex> lock(m);
	return q.empty();
}

template<class T>
inline T ThreadSafeQueue<T>::front()
{
	std::lock_guard<std::mutex> lock(m);
	return q.front();
}

template<class T>
inline void ThreadSafeQueue<T>::pop()
{
	std::lock_guard<std::mutex> lock(m);
	q.pop();
}
