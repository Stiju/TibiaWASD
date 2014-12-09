#ifndef MEMORYSCANNER_H
#define MEMORYSCANNER_H

#include <Windows.h>
#include <vector>
#include <algorithm>
#include "Console.h"

template<class T>
class MemoryScanner {
public:
	void SearchFirst(const T value) {
		DbgLog("SearchFirst\n");
		m_results.clear();
		T *ptr = m_pStart;
		while(ptr < m_pStop) {
			if(!IsBadReadPtr(ptr, sizeof(T)) && *ptr == value)
				m_results.push_back(ptr);
			ptr++;
		}
		DbgLog("Found: %d\n", m_results.size());
	}
	void SearchNext(const T value) {
		DbgLog("SearchNext\n");
		m_results.erase(std::remove_if(m_results.begin(), m_results.end(), [value](T* data) {
			DbgLog("%.8X == %d\n", data, *data);
			return *data != value;
		}), m_results.end());
		DbgLog("Found: %d\n", m_results.size());
	}
	void SetSearchSpan(void *start, void *stop) { m_pStart = static_cast<T*>(start); m_pStop = static_cast<T*>(stop); }

	T* GetFirstResult() const { return m_results.size() > 0 ? m_results.front() : nullptr; }
	int NumberOfResults() const { return m_results.size(); }
private:
	T *m_pStart, *m_pStop;
	std::vector<T*> m_results;
};

#endif