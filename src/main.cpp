#include "vulkan/Vulkan.hpp"
#include "sys/rational.hpp"
#include <iostream>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <glm/glm.hpp>
#pragma GCC diagnostic pop

int main(int argc, char *argv[]) {
	/* Disable validation, ensure Vulkan DSK is installed before setting to true */
	Vulkan *vulkan = new Vulkan(true);

	std::cout << sizeof(rational) << std::endl;

	delete vulkan;
	return 0;
}