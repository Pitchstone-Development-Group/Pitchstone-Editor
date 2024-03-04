#include "Shader.hpp"
#include <iostream>
#include <fstream>

Shader::Shader() {

}

Shader::~Shader() {

}

std::vector<uint32_t> Shader::loadShaderFile(std::string fileName) {
	std::ifstream file(fileName, std::ios::ate | std::ios::binary);
	if (!file.is_open()) {
		throw std::runtime_error("File not found: " + fileName);
	}
	size_t fileSize = (size_t) file.tellg();
	std::vector<uint32_t> shader(fileSize >> 2);
	file.seekg(0);
	file.read((char*)shader.data(), fileSize);
	file.close();
	return shader;
}