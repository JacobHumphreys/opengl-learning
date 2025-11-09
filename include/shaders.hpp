#pragma once

#include "GL/glew.h"
#include <cstdint>
#include <string>
#include <vector>

namespace shaders {

struct ShaderProgramSource {
    std::string vertex_source;
    std::string fragment_source;
};

uint32_t compileShader(uint32_t type, const std::vector<std::string>& sources);
uint32_t compileShader(uint32_t type, const std::string& source);

uint32_t createShader(const ShaderProgramSource source);

ShaderProgramSource parseShader(const std::string& file_path);

}
