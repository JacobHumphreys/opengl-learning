#include <fstream>
#include <iostream>
#include <ranges>
#include <sstream>
#include "errors.hpp"

#include "shaders.hpp"

using shaders::ShaderProgramSource;

using std::string;
using std::vector;

namespace ranges = std::ranges;

uint32_t shaders::compileShader(uint32_t type, const vector<string>& sources) {
    uint32_t id = glCreateShader(type);

    auto raw_sources = sources
        | ranges::views::transform([](const string& s) { return s.c_str(); })
        | ranges::to<vector<const char*>>();

    CALL_GL(glShaderSource(id, raw_sources.size(), raw_sources.data(), nullptr));
    CALL_GL(glCompileShader(id));

    int32_t result;
    CALL_GL(glGetShaderiv(id, GL_COMPILE_STATUS, &result));

    if (result == GL_FALSE) {
        int32_t length;
        CALL_GL(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        auto message = static_cast<char*>(alloca(sizeof(char) * static_cast<size_t>(length)));
        CALL_GL(glGetShaderInfoLog(id, length, &length, message));
        println(std::cerr);
        println(std::cerr, "Shader Compilation Error on shader type: {}", type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");
        println(std::cerr, "Error: {}", message);
        println(std::cerr);
    }

    return id;
}

uint32_t shaders::compileShader(uint32_t type, const string& source) {
    const vector<string> sources = { source };
    return compileShader(type, sources);
}

uint32_t shaders::createShader(const ShaderProgramSource source) {
    uint32_t program = glCreateProgram();

    uint32_t vs = compileShader(GL_VERTEX_SHADER, source.vertex_source);
    uint32_t fs = compileShader(GL_FRAGMENT_SHADER, source.fragment_source);
    CALL_GL(glAttachShader(program, vs));
    CALL_GL(glAttachShader(program, fs));
    CALL_GL(glLinkProgram(program));
    CALL_GL(glValidateProgram(program));

    CALL_GL(glDeleteShader(vs));
    CALL_GL(glDeleteShader(fs));

    return program;
}

ShaderProgramSource shaders::parseShader(const string& file_path) {
    std::ifstream stream(file_path);

    enum class ShaderType {
        none = -1,
        vertex = 0,
        fragment = 1,
    };

    string line;

    std::stringstream shaders[] = { std::stringstream(), std::stringstream() };

    ShaderType type = ShaderType::none;
    while (getline(stream, line)) {
        if (line.find("#shader") != string::npos) {
            if (line.find("vertex") != string::npos) {
                type = ShaderType::vertex;
            } else if (line.find("fragment") != string::npos) {
                type = ShaderType::fragment;
            }
            continue;
        }

        if (type == ShaderType::none) {
            continue;
        }

        shaders[static_cast<size_t>(type)] << line << "\n";
    }

    return {
        .vertex_source = shaders[0].str(),
        .fragment_source = shaders[1].str(),
    };
}
