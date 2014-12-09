#include "Updater.h"
#include "MemoryScanner.h"
#include "TibiaWASD.h"

Updater::Updater(HWND hWnd) {
	m_hWnd = hWnd;
	m_pMemoryScanner = new MemoryScanner<unsigned int>();

	DWORD imageBase = (DWORD)GetModuleHandle(0);
	PIMAGE_DOS_HEADER pimage_dos_header;
	PIMAGE_NT_HEADERS pimage_nt_headers;
	pimage_dos_header = (PIMAGE_DOS_HEADER)imageBase;
	pimage_nt_headers = (PIMAGE_NT_HEADERS)(imageBase + pimage_dos_header->e_lfanew);

	void *start = (void*)(imageBase);
	void *end = (void*)(imageBase + pimage_nt_headers->OptionalHeader.SizeOfImage);
	m_pMemoryScanner->SetSearchSpan(start, end);
}

Updater::~Updater(void) {
	delete m_pMemoryScanner;
}

unsigned int Updater::GetConnectionStatus() {
	return Search("Login and then click OK",
		"Logout and then click OK",
		kConnectionStatusIsOnline, 0);
}

unsigned int Updater::GetActionState() {
	return Search("Open a window (e.g. Options or Info) and then click OK",
		"Close the window and then click OK",
		kActionStateModuleWindowOpen, 0);
}

unsigned int Updater::Search(const char *msg1, const char *msg2, unsigned int val1, unsigned int val2) {
	int repeat = 4, first = 1, mbret;
	do {
		mbret = MessageBox(m_hWnd, msg1, tibiaWasd, MB_OKCANCEL | MB_ICONINFORMATION);
		if(mbret == IDCANCEL)
			return 0;
		if(first) {
			first ^= 1;
			m_pMemoryScanner->SearchFirst(val1);
		} else
			m_pMemoryScanner->SearchNext(val1);
		mbret = MessageBox(m_hWnd, msg2, tibiaWasd, MB_OKCANCEL | MB_ICONINFORMATION);
		if(mbret == IDCANCEL)
			return 0;
		m_pMemoryScanner->SearchNext(val2);
	} while(m_pMemoryScanner->NumberOfResults() > 1 && repeat-- > 0);
	if(m_pMemoryScanner->NumberOfResults() == 0)
		return 0;
	return ((unsigned int)m_pMemoryScanner->GetFirstResult() - (unsigned int)GetModuleHandle(0));
}