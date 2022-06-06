#pragma once

#include "stdafx.h"

template <class T>
class ThreadSafeQueueWrapper
{
private:
	std::queue<T> q;
	mutable std::mutex m;

public:
	T front();
	void pop();
	void push(T value);
	bool empty();
};