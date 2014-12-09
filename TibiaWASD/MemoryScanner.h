#ifndef MEMORYSCANNER_H
#define MEMORYSCANNER_H

#include <Windows.h>
#include "List.h"
#include "Console.h"

template<class T>
class MemoryScanner {
public:
	void SearchFirst(T value);
	void SearchNext(T value);
	void SetSearchSpan(void *start, void *stop) { m_pStart = start; m_pStop = stop; }

	void *GetFirstResult() { return m_results.Begin() ? m_results.Begin()->Data : 0; }
	int NumberOfResults() { return m_results.Count(); }
private:
	void *m_pStart, *m_pStop;
	List<void*> m_results;
};


template<class T>
void MemoryScanner<T>::SearchFirst(T value) {
	DbgLog("SearchFirst\n");
	m_results.Clear();
	T *ptr = (T*)m_pStart;
	while(ptr < (T*)m_pStop) {
		if(!IsBadReadPtr(ptr, sizeof(T)) && *ptr == value)
			m_results.Add((void*)ptr);
		ptr++;
	}
	DbgLog("Found: %d\n", m_results.Count());
}

template<class T>
void MemoryScanner<T>::SearchNext(T value) {
	DbgLog("SearchNext\n");
	for(List<void*>::Node *it = m_results.Begin(); it != 0;) {
		DbgLog("%.8X ", it->Data);
		T *ptr = (T*)(it->Data);
		if(value != *ptr) {
			DbgLog("del\n");
			it = m_results.Del(it);
		} else {
			DbgLog("sav\n");
			it = it->Next;
		}
	}
	DbgLog("Found: %d\n", m_results.Count());
}

#endif