#ifndef CONSOLE_H
#define CONSOLE_H

#if _DEBUG
class CConsole {
public:
	static CConsole *Get(void);
	CConsole(void);
	CConsole(const char *lpTitle);
	~CConsole(void);
	void Write(const char *pMessage, ...);
	int Read(char *pBuffer, int numberOfCharsToRead);
private:
	HANDLE hConsoleOutput, hConsoleInput;
	static CConsole *m_pConsole;
};
#define DbgLog(msg, ...) CConsole::Get()->Write(msg, ## __VA_ARGS__);
#else
#define DbgLog(msg, ...)
#endif

#endif