#ifndef SETTINGS_H
#define SETTINGS_H

#include <vector>
#include <string>

struct FileVersion {
	FileVersion() : Version{0} {}
	union {
		struct {
			unsigned int VersionLo;
			unsigned int VersionHi;
		}u32;
		unsigned long long Version;
	};

	inline bool operator == (const FileVersion &fv) const {
		return Version == fv.Version;
	}
};

struct VersionInfo {
	VersionInfo() : ConnectionStatus{0}, ActionState{0} {}
	FileVersion FileVersion;
	unsigned int ConnectionStatus;
	unsigned int ActionState;

	inline bool operator == (const VersionInfo &vi) const {
		return FileVersion == vi.FileVersion;
	}
};

class Settings {
public:
	Settings() : m_initialized{false} {};

	void Init(const FileVersion &fileVersion, const char *path);
	void LoadDefault();
	void Load();
	void Save();
	bool IsInitialized() const { return m_initialized; }

	VersionInfo CurrentVI;

	struct {
		bool IsVersionIndependent : 1;
		bool IsWasdTitle : 1;

		struct {
			unsigned short North;
			unsigned short East;
			unsigned short South;
			unsigned short West;
			unsigned short NorthEast;
			unsigned short NorthWest;
			unsigned short SouthEast;
			unsigned short SouthWest;
			unsigned short F1, F2, F3, F4, F5, F6;
			unsigned short F7, F8, F9, F10, F11, F12;
			unsigned short MMouseButton;
			unsigned short XMouseButton1;
			unsigned short XMouseButton2;
			unsigned short WriteMode;
			unsigned short Cancel;
		}Keys;
	}Config;

private:
	bool m_initialized;
	std::vector<VersionInfo> m_versions;
	std::string m_file;
	void UpdateVersion();
};

#endif