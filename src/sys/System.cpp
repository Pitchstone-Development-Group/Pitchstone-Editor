#include "System.hpp"

#ifdef _WIN32
#include <windows.h>
#include <userenv.h>
#include <locale>
#include <codecvt>
#include <shlobj.h>
#include <psapi.h>
#include <knownfolders.h>
#include <intrin.h>
#elif defined(__linux__)
#include <stdlib.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <stdio.h>
#include <cpuid.h>
#else
	#error 
#endif

std::string System::getHomeDirectory() {
#ifdef _WIN32
	WCHAR profilePath[MAX_PATH];
	DWORD size = sizeof(profilePath);
	if (!GetUserProfileDirectoryW(GetCurrentProcess(), profilePath, &size)) {
		return "";
	}
	std::wstring userProfilePathWstr(profilePath);
	int utf8Length = WideCharToMultiByte(CP_UTF8, 0, userProfilePathWstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	std::string userProfilePath(utf8Length, 0);
	WideCharToMultiByte(CP_UTF8, 0, userProfilePathWstr.c_str(), -1, &userProfilePath[0], utf8Length, nullptr, nullptr);
	userProfilePath.pop_back();
	return userProfilePath;
#elif defined(__linux__)
	const char* homeDir = getenv("HOME");
	if (homeDir != nullptr)
		return std::string(homeDir);
	else
		return "";
#endif
}

std::string System::printWorkingDirectory() {
#ifdef _WIN32
	WCHAR currentDir[MAX_PATH];
	if (!GetCurrentDirectoryW(MAX_PATH, currentDir)) {
		return "";
	}
	std::wstring currentDirWstr(currentDir);
	int utf8Length = WideCharToMultiByte(CP_UTF8, 0, currentDirWstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	std::string currentDirUtf8(utf8Length, 0);
	WideCharToMultiByte(CP_UTF8, 0, currentDirWstr.c_str(), -1, &currentDirUtf8[0], utf8Length, nullptr, nullptr);
	currentDirUtf8.pop_back();
	return currentDirUtf8;
#elif defined(__linux__)
	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) != nullptr)
		return std::string(cwd);
	return "";
#endif
}

std::string System::getAppdataDirectory() {
#ifdef _WIN32
	PWSTR path = nullptr;
	if (SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &path) != S_OK) {
		return "";
	}
	std::wstring appDataPathWstr(path);
	CoTaskMemFree(path);
	int utf8Length = WideCharToMultiByte(CP_UTF8, 0, appDataPathWstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	std::string appDataPath(utf8Length, 0);
	WideCharToMultiByte(CP_UTF8, 0, appDataPathWstr.c_str(), -1, &appDataPath[0], utf8Length, nullptr, nullptr);
	appDataPath.pop_back();
	return appDataPath;
#elif defined(__linux__)
	const char* homeDir = getenv("XDG_DATA_HOME");
	if (homeDir != nullptr)
		return std::string(homeDir);
	else
		return getHomeDirectory() + "/.local/share";
#endif
}

void System::cpuid(uint32_t ia, uint32_t ic, uint32_t *oa, uint32_t *ob, uint32_t *oc, uint32_t *od) {
	int info[4];
#ifdef _WIN32
	__cpuidex(info, (int)ia, (int)ic);
#elif defined(__linux__)
	__cpuid_count((int)ia, (int)ic, &info[0], &info[1], &info[2], &info[3]);
#endif
	*oa = (uint32_t)info[0];
	*ob = (uint32_t)info[1];
	*oc = (uint32_t)info[2];
	*od = (uint32_t)info[3];
}

void System::getRAM(uint32_t *TM, uint32_t *AM, uint32_t *CM) {
#ifdef _WIN32
	MEMORYSTATUSEX status;
	HANDLE process = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS_EX pmc;
	status.dwLength = sizeof(status);
	if (GlobalMemoryStatusEx(&status)) {
		*TM = (uint32_t) (status.ullTotalPhys / 1000000ULL);
		*AM = (uint32_t) (status.ullAvailPhys / 1000000ULL);

		if (!GetProcessMemoryInfo(process, reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc), sizeof(pmc))) {
			*CM = 1;
		} else {
			*CM = (uint32_t) (pmc.WorkingSetSize / 1000000ULL);
		}
	} else {
		*TM = 1;
		*AM = 1;
		*CM = 1;
	}
#elif defined(__linux__)
	struct sysinfo memInfo;
	sysinfo(&memInfo);
	*TM = (uint32_t) ((memInfo.totalram * memInfo.mem_unit) / 1000000UL);
	long long freePhysMem = memInfo.freeram * memInfo.mem_unit;
	long long bufferMem = memInfo.bufferram * memInfo.mem_unit;
	long long cacheMem = memInfo.sharedram * memInfo.mem_unit;
	*AM = (uint32_t) ((freePhysMem + bufferMem + cacheMem) / 1000000UL);
	FILE* statm = fopen("/proc/self/statm", "r");
    unsigned long long resident;
    if (fscanf(statm, "%*u %llu", &resident) == 1)
    	*CM = (uint32_t) ((resident * getpagesize()) / 1000000UL);
	else
		*CM = 0;
    fclose(statm);
#endif
}