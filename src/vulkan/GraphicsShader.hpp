#pragma once

#include "Shader.hpp"

class GraphicsShader : public Shader {
public:
	GraphicsShader(Device *device, std::string shaderName, uint32_t pcsize);
	~GraphicsShader();
};