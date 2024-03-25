#pragma once

#include <string>

class System {
public:
	static std::string printWorkingDirectory();
	static bool changeDirectory();
	static std::string getHomeDirectory();
	static std::string getAppdataDirectory();

	static void cpuid(uint32_t ia, uint32_t ic, uint32_t *oa, uint32_t *ob, uint32_t *oc, uint32_t *od);
	static void getRAM(uint32_t *TM, uint32_t *AM, uint32_t *CM);
};