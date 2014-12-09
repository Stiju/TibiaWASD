#ifndef UPDATER_H
#define UPDATER_H

#include <Windows.h>
#include "MemoryScanner.h"

class Updater {
public:
	Updater(HWND hWnd);

	unsigned int GetConnectionStatus();
	unsigned int GetActionState();
private:
	unsigned int Search(const char *msg1, const char *msg2, unsigned int val1, unsigned int val2);
	MemoryScanner<unsigned int> m_memoryScanner;
	HWND m_hWnd;
};

#endif