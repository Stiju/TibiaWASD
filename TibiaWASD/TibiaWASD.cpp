#include <Windows.h>
#include <cstdio>
#include "TibiaWASD.h"
#include "Settings.h"
#include "Console.h"
#include "ConfigWnd.h"

const char *tibiaWasd = "TibiaWASD";

Settings *settings = 0;

HWND g_hWnd = 0;
bool isActive = true;
WNDPROC wpTibiaProc;
HMODULE g_hModule;

char *strrchr(char *str, char ch) {
	char *ptr = 0;
	do {
		if(*str == ch)
			ptr = str;
	} while(*str++);
	return ptr;
}

#if !_DEBUG
int sprintf(char *dst, const char *format, ...) {
	va_list v;
	va_start(v, format);
	return wvsprintfA(dst, format, v);
}
#endif

void DisplayError(DWORD errorcode) {
	char *error, *msg;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		0, errorcode, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), (LPSTR)&error, 0, 0);
	msg = (char*)LocalAlloc(LMEM_ZEROINIT, strlen(error) + 32);
	sprintf(msg, "Error code: %d\n%s", error);
	MessageBox(0, msg, tibiaWasd, MB_ICONEXCLAMATION | MB_TOPMOST);
	LocalFree(msg);
	LocalFree(error);
}

bool GetVersionInfo(FileVersion *fileVersion) {
	char filename[MAX_PATH];
	DWORD pathlength = GetModuleFileName(GetModuleHandle(0), filename, MAX_PATH);

	if(pathlength > MAX_PATH) {
		DbgLog("File path is too big %d\n", pathlength);
		return false;
	}

	DWORD handle, size;
	unsigned int length;
	VS_FIXEDFILEINFO *ffi = 0;
	size = GetFileVersionInfoSize(filename, &handle);
	if(!size) {
		DbgLog("No version information\n");
		return false;
	}
	unsigned char *buffer = new unsigned char[size];
	if(!buffer) {
		DbgLog("Memory allocation failed\n");
		return false;
	}

	if(!GetFileVersionInfo(filename, 0, size, buffer)) {
		DbgLog("Failed to retrieve version info\n");
		delete[] buffer;
		return false;
	}

	if(!VerQueryValueA(buffer, "\\", (void**)&ffi, &length) || !ffi) {
		DbgLog("Could not parse version info query\n");
		delete[] buffer;
		return false;
	}

	if(length != sizeof(VS_FIXEDFILEINFO)) {
		DbgLog("sizeof VS_FIXEDFILEINFO: %d length: %d\n", sizeof(VS_FIXEDFILEINFO), length);
		delete[] buffer;
		return false;
	}

	fileVersion->u32.VersionHi = ffi->dwFileVersionMS;
	fileVersion->u32.VersionLo = ffi->dwFileVersionLS;

	delete[] buffer;

	return true;
}

void UpdateTitle() {
	static const char *tibia01 = "Tibia", *tibia02 = "TibiaWASD",
		*tibiawrite01 = "Tibia [WriteMode]", *tibiawrite02 = "TibiaWASD [WriteMode]";
	const char *title;
	if(settings->Config.IsWasdTitle) {
		if(isActive)
			title = tibia02;
		else
			title = tibiawrite02;
	} else {
		if(isActive)
			title = tibia01;
		else
			title = tibiawrite01;
	}
	SetWindowText(g_hWnd, title);
}

inline bool IsEqualAndNotZero(unsigned short key, WPARAM wParam) {
	return key != 0 && wParam == key;
}

void SwitchLayout(WPARAM& wParam) {
	// MoveKeys
	if(IsEqualAndNotZero(settings->Config.Keys.North, wParam))
		wParam = VK_UP;
	else if(IsEqualAndNotZero(settings->Config.Keys.West, wParam))
		wParam = VK_LEFT;
	else if(IsEqualAndNotZero(settings->Config.Keys.South, wParam))
		wParam = VK_DOWN;
	else if(IsEqualAndNotZero(settings->Config.Keys.East, wParam))
		wParam = VK_RIGHT;
	else if(IsEqualAndNotZero(settings->Config.Keys.NorthWest, wParam))
		wParam = VK_HOME;
	else if(IsEqualAndNotZero(settings->Config.Keys.NorthEast, wParam))
		wParam = VK_PRIOR;
	else if(IsEqualAndNotZero(settings->Config.Keys.SouthWest, wParam))
		wParam = VK_END;
	else if(IsEqualAndNotZero(settings->Config.Keys.SouthEast, wParam))
		wParam = VK_NEXT;
	// HotKeys
	else if(IsEqualAndNotZero(settings->Config.Keys.F1, wParam))
		wParam = VK_F1;
	else if(IsEqualAndNotZero(settings->Config.Keys.F2, wParam))
		wParam = VK_F2;
	else if(IsEqualAndNotZero(settings->Config.Keys.F3, wParam))
		wParam = VK_F3;
	else if(IsEqualAndNotZero(settings->Config.Keys.F4, wParam))
		wParam = VK_F4;
	else if(IsEqualAndNotZero(settings->Config.Keys.F5, wParam))
		wParam = VK_F5;
	else if(IsEqualAndNotZero(settings->Config.Keys.F6, wParam))
		wParam = VK_F6;
	else if(IsEqualAndNotZero(settings->Config.Keys.F7, wParam))
		wParam = VK_F7;
	else if(IsEqualAndNotZero(settings->Config.Keys.F8, wParam))
		wParam = VK_F8;
	else if(IsEqualAndNotZero(settings->Config.Keys.F9, wParam))
		wParam = VK_F9;
	else if(IsEqualAndNotZero(settings->Config.Keys.F10, wParam))
		wParam = VK_F10;
	else if(IsEqualAndNotZero(settings->Config.Keys.F11, wParam))
		wParam = VK_F11;
	else if(IsEqualAndNotZero(settings->Config.Keys.F12, wParam))
		wParam = VK_F12;
}

LRESULT APIENTRY TibiaProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static unsigned int baseAddress = (unsigned int)GetModuleHandle(0);
	static bool isBlocking = false;
	static bool isReady = false;
	static bool isCtrlDown = false;
	static bool isNonBlock = false;
	auto isOnline = []()->bool {
		if(settings) {
			unsigned int *ptr = (unsigned int*)(settings->CurrentVI.ConnectionStatus + baseAddress);
			if(!IsBadReadPtr(ptr, 4))
				return *ptr == kConnectionStatusIsOnline;
		}
		return true;
	};
	auto isModuleWindowOpen = []()->bool {
		if(settings) {
			unsigned int *ptr = (unsigned int*)(settings->CurrentVI.ActionState + baseAddress);
			if(!IsBadReadPtr(ptr, 4))
				return *ptr == kActionStateModuleWindowOpen;
		}
		return true;
	};

	if(!isReady) {
		isReady = true;
		DbgLog("DebugMode\n");

		char path[MAX_PATH];
		GetModuleFileName(g_hModule, path, MAX_PATH);
		char *c = strrchr(path, '\\');
		if(c != 0)
			*++c = 0;

		FileVersion fileVersion = {0};
		if(!GetVersionInfo(&fileVersion))
			DisplayError(GetLastError());

		settings = new Settings(fileVersion, path);
		settings->Load();

		UpdateTitle();
		InitConfigWnd(&g_hModule);
	}

	switch(uMsg) {
	case WM_CHAR:
		DbgLog("WM_CHAR %.8X %.8X\n", wParam, lParam);
		if(isNonBlock) {
			isNonBlock = false;
		} else if(settings->Config.IsVersionIndependent || isOnline() && !isModuleWindowOpen()) {
			if(!isBlocking && isActive)
				return 0;
			if(isBlocking) {
				isBlocking = false;
				if(!isActive)
					return 0;
			}
		}
		break;
	case WM_KEYDOWN:
		if(wParam == VK_CONTROL)
			isCtrlDown = true;
		DbgLog("WM_KEYDOWN %.8X %.8X\n", wParam, lParam);
		if(!isActive) {
			if(wParam == VK_RETURN || IsEqualAndNotZero(settings->Config.Keys.Cancel, wParam)) {
				isActive = true;
				isBlocking = true;
				UpdateTitle();
				break;
			}
		} else if(settings->Config.IsVersionIndependent || isActive && isOnline() && !isModuleWindowOpen()) {
			if(IsEqualAndNotZero(settings->Config.Keys.WriteMode, wParam)) {
				isActive = false;
				isBlocking = true;
				UpdateTitle();
			} else if(isCtrlDown && (wParam == 'G' || wParam == 'L' || wParam == 'R')) {
				isNonBlock = true;
			} else {
				SwitchLayout(wParam);
			}
		}
		break;
	case WM_KEYUP:
		if(wParam == VK_CONTROL) {
			isCtrlDown = false;
		} else {
			SwitchLayout(wParam);
		}
		break;
	case WM_MBUTTONDOWN:
		if(settings->Config.Keys.MMouseButton != 0) {
			SendMessage(hWnd, WM_KEYDOWN, settings->Config.Keys.MMouseButton, 0);
		}
		break;
	case WM_MBUTTONUP:
		if(settings->Config.Keys.MMouseButton != 0) {
			SendMessage(hWnd, WM_KEYUP, settings->Config.Keys.MMouseButton, 0);
		}
		break;
	case WM_XBUTTONDOWN:
		if(settings->Config.Keys.XMouseButton1 != 0 && HIWORD(wParam) == XBUTTON1) {
			SendMessage(hWnd, WM_KEYDOWN, settings->Config.Keys.XMouseButton1, 0);
		} else if(settings->Config.Keys.XMouseButton2 != 0 && HIWORD(wParam) == XBUTTON2) {
			SendMessage(hWnd, WM_KEYDOWN, settings->Config.Keys.XMouseButton2, 0);
		}
		break;
	case WM_XBUTTONUP:
		if(settings->Config.Keys.XMouseButton1 != 0 && HIWORD(wParam) == XBUTTON1) {
			SendMessage(hWnd, WM_KEYUP, settings->Config.Keys.XMouseButton1, 0);
		} else if(settings->Config.Keys.XMouseButton2 != 0 && HIWORD(wParam) == XBUTTON2) {
			SendMessage(hWnd, WM_KEYUP, settings->Config.Keys.XMouseButton2, 0);
		}
		break;
	case WM_DESTROY:
		DestroyConfigWnd();
		delete settings;
		break;
	}
	return CallWindowProc(wpTibiaProc, hWnd, uMsg, wParam, lParam);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID) {
	HWND hWnd = 0;
	switch(ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		g_hModule = hModule;
		DisableThreadLibraryCalls(g_hModule);

		while((hWnd = FindWindowEx(0, hWnd, "TibiaClient", 0)) != 0) {
			DWORD dwPid;
			GetWindowThreadProcessId(hWnd, &dwPid);
			if(dwPid == GetCurrentProcessId()) {
				g_hWnd = hWnd;
				break;
			}
		}
		if(!g_hWnd)
			break;
		wpTibiaProc = (WNDPROC)SetWindowLongPtr(g_hWnd, GWLP_WNDPROC, (LONG)TibiaProc);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}