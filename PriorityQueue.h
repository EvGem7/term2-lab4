#pragma once
#include <queue>

template <class T>
class PriorityQueue : public std::priority_queue<T> {
public:
	T poll() {
		T result = top();
		pop();
		return result;
	}
};

