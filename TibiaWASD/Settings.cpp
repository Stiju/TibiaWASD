#include <Windows.h>
#include <algorithm>
#include "Settings.h"
#include "Console.h"
#include "TibiaWASD.h"

void Settings::Init(const FileVersion &fileVersion, const char *path) {
	CurrentVI.FileVersion = fileVersion;
	m_file = std::string(path) + std::string("TibiaWASD.dat");
	LoadDefault();
	m_initialized = true;
}

void Settings::LoadDefault() {
	Config.IsVersionIndependent = 0;
	Config.IsWasdTitle = 1;
	Config.Keys.North = 'W';
	Config.Keys.East = 'D';
	Config.Keys.South = 'S';
	Config.Keys.West = 'A';
	Config.Keys.NorthEast = 'E';
	Config.Keys.NorthWest = 'Q';
	Config.Keys.SouthEast = 'X';
	Config.Keys.SouthWest = 'Z';
	Config.Keys.F1 = '1';
	Config.Keys.F2 = '2';
	Config.Keys.F3 = '3';
	Config.Keys.F4 = '4';
	Config.Keys.F5 = '5';
	Config.Keys.F6 = '6';
	Config.Keys.F7 = '7';
	Config.Keys.F8 = '8';
	Config.Keys.F9 = '9';
	Config.Keys.F10 = '0';
	Config.Keys.F11 = 0;
	Config.Keys.F12 = 0;
	Config.Keys.MMouseButton = VK_F10;
	Config.Keys.XMouseButton1 = VK_F11;
	Config.Keys.XMouseButton2 = VK_F12;
	Config.Keys.WriteMode = 'Y';
	Config.Keys.Cancel = VK_ESCAPE;
}

void Settings::Load() {
	m_versions.clear();
	HANDLE hFile;
	hFile = CreateFile(m_file.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if(hFile != INVALID_HANDLE_VALUE) {
		bool currentVersion = false;
		WORD count = 0;
		DWORD read = 0;
		ReadFile(hFile, &Config, sizeof(Config), &read, 0);
		ReadFile(hFile, &count, 2, &read, 0);
		for(int i = 0; i < count; i++) {
			VersionInfo vi;
			ReadFile(hFile, &vi, sizeof(vi), &read, 0);
			m_versions.push_back(vi);
			if(vi.FileVersion == CurrentVI.FileVersion) {
				CurrentVI.ActionState = vi.ActionState;
				CurrentVI.ConnectionStatus = vi.ConnectionStatus;
				currentVersion = true;
			}
		}
		CloseHandle(hFile);

		if(!currentVersion) {
			m_versions.push_back(CurrentVI);
		}
	} else {
		DWORD error = GetLastError();
		if(error == ERROR_FILE_NOT_FOUND) {
			Save();
			return;
		}
		DisplayError(error);
	}
}

void Settings::Save() {
	UpdateVersion();
	HANDLE hFile;
	hFile = CreateFile(m_file.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if(hFile != INVALID_HANDLE_VALUE) {
		WORD count = (WORD)m_versions.size();
		DWORD written = 0;
		WriteFile(hFile, &Config, sizeof(Config), &written, 0);
		WriteFile(hFile, &count, 2, &written, 0);

		for(const auto& version : m_versions) {
			WriteFile(hFile, &version, sizeof(VersionInfo), &written, 0);
		}
		CloseHandle(hFile);
	} else {
		DisplayError(GetLastError());
	}
}

void Settings::UpdateVersion() {
	auto vi = std::find(m_versions.begin(), m_versions.end(), CurrentVI);
	if(vi != m_versions.end()) {
		vi->ConnectionStatus = CurrentVI.ConnectionStatus;
		vi->ActionState = CurrentVI.ActionState;
		return;
	}
	m_versions.push_back(CurrentVI);
}
