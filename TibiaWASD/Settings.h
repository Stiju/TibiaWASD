#ifndef SETTINGS_H
#define SETTINGS_H

#include "List.h"

struct FileVersion {
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
	FileVersion FileVersion;
	unsigned int ConnectionStatus;
	unsigned int ActionState;

	inline bool operator == (const VersionInfo &vi) const {
		return FileVersion == vi.FileVersion;
	}
};

class Settings {
public:
	Settings(const FileVersion &fileVersion, const char *path);
	~Settings(void);

	void LoadDefault();
	int Load();
	int Save();

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
	List<VersionInfo> m_version;
	char *m_pFile;

	void UpdateVersion();
};

#endif