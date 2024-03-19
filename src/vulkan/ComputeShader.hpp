#pragma once

#include "Shader.hpp"

class ComputeShader : public Shader {
public:
	ComputeShader(Device *device, std::string shaderName, uint32_t pcsize);
	~ComputeShader();
};