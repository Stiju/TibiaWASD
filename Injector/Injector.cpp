#include <memory>
#include <windows.h>

const int kMaxRetries = 6;
const char kDllName[] = "TibiaWASD.dll";
const char kProgramTitle[] = "TibiaWASD Injector";

int DisplayError(const char *function, bool lasterror = true) {
	if(lasterror) {
		DWORD errorcode = GetLastError();
		char *error = nullptr, *msg = nullptr;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			0, errorcode, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), (LPSTR)&error, 0, 0);
		msg = (char*)LocalAlloc(LMEM_ZEROINIT, std::strlen(error) + 64);
		wsprintf(msg, "%s returned error: %d\n%s", function, errorcode, error);
		MessageBox(0, msg, kProgramTitle, MB_ICONEXCLAMATION | MB_TOPMOST);
		LocalFree(msg);
		LocalFree(error);
	} else {
		MessageBox(0, function, kProgramTitle, MB_ICONEXCLAMATION | MB_TOPMOST);
	}
	return -1;
}

int InjectLibrary(DWORD dwProcessId, char* dllPath) {
	using unique_handle = std::unique_ptr < std::remove_pointer<HANDLE>::type, decltype(&CloseHandle) > ;
	auto length = std::strlen(dllPath) + 1;

	unique_handle hProcess(OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, false, dwProcessId), CloseHandle);
	if(hProcess == 0) {
		return DisplayError("OpenProcess");
	}

	auto virtualFree = [&](LPVOID address) {
		if(address != 0) {
			VirtualFreeEx(hProcess.get(), address, 0, MEM_RELEASE);
		}
	};
	using unique_vrtlmem = std::unique_ptr < std::remove_pointer<LPVOID>::type, decltype(virtualFree) > ;
	unique_vrtlmem remoteAddress(VirtualAllocEx(hProcess.get(), 0, length, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE), virtualFree);
	if(remoteAddress == 0) {
		return DisplayError("VirtualAllocEx");
	}
	if(WriteProcessMemory(hProcess.get(), remoteAddress.get(), (LPVOID)dllPath, length, 0) == 0) {
		return DisplayError("WriteProcessMemory");
	}
	unique_handle hThread(CreateRemoteThread(hProcess.get(), 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, remoteAddress.get(), 0, 0), CloseHandle);
	if(hThread == 0) {
		return DisplayError("CreateRemoteThread");
	}
	WaitForSingleObject(hThread.get(), INFINITE);
	return 0;
}

bool CheckDll(char *dllPath) {
	if(GetModuleFileNameA(0, dllPath, MAX_PATH) != 0) {
		char *ptr = std::strrchr(dllPath, '\\');
		if(ptr != NULL)
			*++ptr = 0;
		strcat_s(dllPath, MAX_PATH, kDllName);
		return GetFileAttributesA(dllPath) != INVALID_FILE_ATTRIBUTES;
	}
	return false;
}

int main() {
	HWND hWnd;
	DWORD pid;
	char dllPath[MAX_PATH];
	if(CheckDll(dllPath)) {
		for(int i = 0; i < kMaxRetries; ++i) {
			hWnd = FindWindowA("TibiaClient", 0);
			if(hWnd) {
				GetWindowThreadProcessId(hWnd, &pid);
				return InjectLibrary(pid, dllPath);
			}
			Sleep(1000);
		}
		return DisplayError("No Tibia client found.", false);
	}
	return DisplayError("The system cannot find TibiaWASD.dll.", false);
}

