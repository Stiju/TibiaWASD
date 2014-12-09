#if _DEBUG

#include <windows.h>
#include <cstdio>
#include "Console.h"

CConsole *CConsole::m_pConsole = 0;

CConsole *CConsole::Get(void) {
	if(!m_pConsole)
		m_pConsole = new CConsole("Debug Console");
	return m_pConsole;
}

CConsole::CConsole(void) {
	AllocConsole();
	hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	hConsoleInput = GetStdHandle(STD_INPUT_HANDLE);
}

CConsole::CConsole(const char *lpTitle) {
	AllocConsole();
	hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	hConsoleInput = GetStdHandle(STD_INPUT_HANDLE);
	SetConsoleTitle(lpTitle);
}

CConsole::~CConsole(void) {
	FreeConsole();
}

void CConsole::Write(const char *pMessage, ...) {
	int len;
	char *buffer;
	va_list args;
	va_start(args, pMessage);

	len = _vscprintf(pMessage, args) + 1;
	buffer = new char[len];
	vsprintf_s(buffer, len, pMessage, args);

	WriteConsole(hConsoleOutput, buffer, strlen(buffer), 0, 0);

	delete[] buffer;
	va_end(args);
}

int CConsole::Read(char *pBuffer, int numberOfCharsToRead) {
	DWORD read;
	memset(pBuffer, 0, numberOfCharsToRead);
	ReadConsole(hConsoleInput, pBuffer, numberOfCharsToRead, &read, 0);
	return read;
}

#endif