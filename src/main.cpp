#include "vulkan/Vulkan.hpp"
#include "sys/rational.hpp"
#include <iostream>

#include <glm/glm.hpp>
#include <cpuid.h>

static inline void cpuid(uint32_t arg1, uint32_t arg2, uint32_t *regs) {
	__asm__ __volatile__ (
		"cpuid"
		: "=a" (regs[0]), "=b" (regs[1]), "=c" (regs[2]), "=d" (regs[3])
		: "a" (arg1), "c" (arg2)
	);
}

int main(int argc, char *argv[]) {
	/* Disable validation, ensure Vulkan DSK is installed before setting to true */
	Vulkan *vulkan = new Vulkan(true);

	std::vector<VkPhysicalDevice> physicals;
	/* Count and then list physical devices available, then choose the first one */
	uint32_t count;
	vkEnumeratePhysicalDevices(vulkan->instance, &count, NULL);
	physicals.resize(count);
	std::cout << count << " number of graphics devices" << std::endl;
	vkEnumeratePhysicalDevices(vulkan->instance, &count, physicals.data());

	uint32_t ret[4];

	cpuid(0, 0, ret);

	std::cout << ret[0] << " " << ret[1] << " " << ret[2] << " " << ret[3] << std::endl;

	delete vulkan;
	return 0;
}