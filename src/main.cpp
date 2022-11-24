#include "vulkan/Vulkan.hpp"

int main(int argc, char *argv[]) {
	/* Disable validation, ensure Vulkan DSK is installed before setting to true */
	Vulkan *vulkan = new Vulkan(true);

	delete vulkan;
	return 0;
}