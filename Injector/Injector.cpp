#include <windows.h>

#ifndef _DEBUG
#pragma comment(linker, "/ENTRY:main /NODEFAULTLIB")
#endif

const char szDllName[] = "TibiaWASD.dll";
const char szTitle[] = "TibiaWASD Injector";

// User32 functions
typedef HWND (WINAPI *FINDWINDOW)(LPCSTR, LPSTR);
typedef int (WINAPI *MESSAGEBOX)(HWND,LPCSTR,LPCSTR,UINT);
typedef DWORD (WINAPI *GETWINDOWTHREADPROCESSID)(HWND,LPDWORD);
typedef int (WINAPI *WSPRINTF)(LPSTR,LPCSTR,...);

FINDWINDOW iFindWindow;
MESSAGEBOX iMessageBox;
GETWINDOWTHREADPROCESSID iGetWindowThreadProcessId;
WSPRINTF iwsprintf;

// Kernel32 functions
typedef DWORD (WINAPI *GETFILEATTRIBUTES)(LPCSTR);
typedef BOOL (WINAPI *CLOSEHANDLE)(HANDLE);
typedef DWORD (WINAPI *GETLASTERROR)();
typedef DWORD (WINAPI *WAITFORSINGLEOBJECT)(HANDLE,DWORD);
typedef VOID (WINAPI *SLEEP)(DWORD);
typedef HANDLE (WINAPI *CREATEREMOTETHREAD)(HANDLE,LPSECURITY_ATTRIBUTES,SIZE_T,LPTHREAD_START_ROUTINE,LPVOID,DWORD,LPDWORD);
typedef HANDLE (WINAPI *OPENPROCESS)(DWORD,BOOL,DWORD);
typedef LPVOID (WINAPI *VIRTUALALLOCEX)(HANDLE,LPVOID,SIZE_T,DWORD,DWORD);
typedef BOOL (WINAPI *VIRTUALFREEEX)(HANDLE,LPVOID,SIZE_T,DWORD);
typedef BOOL (WINAPI *WRITEPROCESSMEMORY)(HANDLE,LPVOID,LPCVOID,SIZE_T,SIZE_T);
typedef HLOCAL (WINAPI *LOCALALLOC)(UINT,SIZE_T);
typedef HLOCAL (WINAPI *LOCALFREE)(HLOCAL);
typedef DWORD (WINAPI *FORMATMESSAGE)(DWORD,LPCVOID,DWORD,DWORD,LPSTR,DWORD,va_list);
typedef DWORD (WINAPI *GETMODULEFILENAME)(HMODULE,LPSTR,DWORD);
typedef HMODULE (WINAPI *LOADLIBRARY)(LPCSTR);

GETFILEATTRIBUTES iGetFileAttributes;
CLOSEHANDLE iCloseHandle;
GETLASTERROR iGetLastError;
WAITFORSINGLEOBJECT iWaitForSingleObject;
SLEEP iSleep;
CREATEREMOTETHREAD iCreateRemoteThread;
OPENPROCESS iOpenProcess;
VIRTUALALLOCEX iVirtualAllocEx;
VIRTUALFREEEX iVirtualFreeEx;
WRITEPROCESSMEMORY iWriteProcessMemory;
LOCALALLOC iLocalAlloc;
LOCALFREE iLocalFree;
FORMATMESSAGE iFormatMessage;
GETMODULEFILENAME iGetModuleFileName;
LOADLIBRARY iLoadLibrary;

inline int istrlen(const char *str) {
	const char *p = str;
	while(*p) p++;
	return p - str;
}

inline char *istrcat(char *dest, const char *src) {
	char *p = dest;
	while(*p) p++;
	while((*p++ = *src++) != 0);
	return dest;
}

inline char *istrrchr(const char *str, char ch) {
	const char *ptr = 0;
	do {
		if(*str == ch)
			ptr = str;
	} while(*str++);
	return (char*)ptr;
}

void InitFunctions() {
	HMODULE kernel32 = GetModuleHandle("kernel32");
	iGetFileAttributes = (GETFILEATTRIBUTES)GetProcAddress(kernel32, "GetFileAttributesA");
	iCloseHandle = (CLOSEHANDLE)GetProcAddress(kernel32, "CloseHandle");
	iGetLastError = (GETLASTERROR)GetProcAddress(kernel32, "GetLastError");
	iWaitForSingleObject = (WAITFORSINGLEOBJECT)GetProcAddress(kernel32, "WaitForSingleObject");
	iSleep = (SLEEP)GetProcAddress(kernel32, "Sleep");
	iCreateRemoteThread = (CREATEREMOTETHREAD)GetProcAddress(kernel32, "CreateRemoteThread");
	iOpenProcess = (OPENPROCESS)GetProcAddress(kernel32, "OpenProcess");
	iVirtualAllocEx = (VIRTUALALLOCEX)GetProcAddress(kernel32, "VirtualAllocEx");
	iVirtualFreeEx = (VIRTUALFREEEX)GetProcAddress(kernel32, "VirtualFreeEx");
	iWriteProcessMemory = (WRITEPROCESSMEMORY)GetProcAddress(kernel32, "WriteProcessMemory");
	iLocalAlloc = (LOCALALLOC)GetProcAddress(kernel32, "LocalAlloc");
	iLocalFree = (LOCALFREE)GetProcAddress(kernel32, "LocalFree");
	iFormatMessage = (FORMATMESSAGE)GetProcAddress(kernel32, "FormatMessageA");
	iGetModuleFileName = (GETMODULEFILENAME)GetProcAddress(kernel32, "GetModuleFileNameA");
	iLoadLibrary = (LOADLIBRARY)GetProcAddress(kernel32, "LoadLibraryA");

	HMODULE user32 = iLoadLibrary("user32");
	iFindWindow = (FINDWINDOW)GetProcAddress(user32, "FindWindowA");
	iMessageBox = (MESSAGEBOX)GetProcAddress(user32, "MessageBoxA");
	iGetWindowThreadProcessId = (GETWINDOWTHREADPROCESSID)GetProcAddress(user32, "GetWindowThreadProcessId");
	iwsprintf = (WSPRINTF)GetProcAddress(user32, "wsprintfA");
}

int DisplayError(char *function, bool lasterror = true) {
	if(lasterror) {
		DWORD errorcode = iGetLastError();
		char *error, *msg;
		iFormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			0, errorcode, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), (LPSTR)&error, 0, 0);
		msg = (char*)iLocalAlloc(LMEM_ZEROINIT, istrlen(error) + 64);
		iwsprintf(msg, "%s returned error: %d\n%s", function, errorcode, error);
		iMessageBox(0, msg, szTitle, MB_ICONEXCLAMATION | MB_TOPMOST);
		iLocalFree(msg);
		iLocalFree(error);
	} else {
		iMessageBox(0, function, szTitle, MB_ICONEXCLAMATION | MB_TOPMOST);
	}
	return -1;
}

int InjectLibrary(DWORD dwProcessId, char* dllPath) {
	int errc = 0;
	HANDLE hProcess = 0, hThread = 0;
	LPVOID lpRemoteAddress = 0;

	hProcess = iOpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, false, dwProcessId);
	if(hProcess == 0)
		return DisplayError("OpenProcess");
	int length = istrlen(dllPath) + 1;
	lpRemoteAddress = iVirtualAllocEx(hProcess, 0, length, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if(lpRemoteAddress != 0) {
		int ret = iWriteProcessMemory(hProcess, lpRemoteAddress, (LPVOID)dllPath, length, 0);
		if(ret != 0) {
			hThread = iCreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE)iLoadLibrary, lpRemoteAddress, 0, 0);
			if(hThread != 0) {
				iWaitForSingleObject(hThread, INFINITE);
			} else {
				errc = DisplayError("CreateRemoteThread");
			}
		} else {
			errc = DisplayError("WriteProcessMemory");
		}
	} else {
		errc = DisplayError("VirtualAllocEx");
	}

	if(lpRemoteAddress)
		iVirtualFreeEx(hProcess, lpRemoteAddress, 0, MEM_RELEASE);
	if(hProcess)
		iCloseHandle(hProcess);

	return errc;
}

bool CheckDll(char *dllPath) {
	if(iGetModuleFileName(0, dllPath, MAX_PATH) != 0) {
		char *ptr = istrrchr(dllPath, '\\');
		if(ptr != NULL)
			*++ptr = 0;
		istrcat(dllPath, szDllName);
		return iGetFileAttributes(dllPath) != INVALID_FILE_ATTRIBUTES;
	}
	return false;
}

int main() {
	HWND hWnd;
	DWORD pid;
	char dllPath[MAX_PATH];
	int errc = 0;
	InitFunctions();
	if(CheckDll(dllPath)) {
		for(int i = 0; i < 5; i++) {
			if(i != 0)
				iSleep(1000);
			hWnd = iFindWindow("TibiaClient", 0);
			if(hWnd) {
				iGetWindowThreadProcessId(hWnd, &pid);

				return InjectLibrary(pid, dllPath);
			}
		}
		errc = DisplayError("No Tibia client found.", false);
	} else {
		errc = DisplayError("The system cannot find TibiaWASD.dll.", false);
	}

	return errc;
}

