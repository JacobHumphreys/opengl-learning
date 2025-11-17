#include "renderer/Shader.hpp"
#include "errors.hpp"
#include "fstream"
#include "sstream"
#include "iostream"
#include <cstdint>
#include <ostream>
#include <stdexcept>

using std::string;

struct ShaderProgramSource {
    std::string vertex_source;
    std::string fragment_source;

    ShaderProgramSource(const string& file_path) {
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

        vertex_source = shaders[0].str();
        fragment_source = shaders[1].str();
    }
};

Shader::Shader(const string& file_path) {
    auto source = ShaderProgramSource(file_path);

    renderer_id = glCreateProgram();
    uint32_t vertex = compileShader(GL_VERTEX_SHADER, source.vertex_source);
    uint32_t fragment = compileShader(GL_FRAGMENT_SHADER, source.fragment_source);

    CALL_GL(glAttachShader(renderer_id, vertex));
    CALL_GL(glAttachShader(renderer_id, fragment));
    CALL_GL(glLinkProgram(renderer_id));
    CALL_GL(glValidateProgram(renderer_id));

    CALL_GL(glDeleteShader(vertex));
    CALL_GL(glDeleteShader(fragment));
}

Shader::~Shader() {
    CALL_GL(glDeleteProgram(renderer_id))
}

void Shader::setUniform4f(const string& name, float4 value) {
    CALL_GL(glUniform4f(getUniformLocation(name), value.r, value.g, value.b, value.a));
}

void Shader::bind() const {
    CALL_GL(glUseProgram(renderer_id))
}
void Shader::unbind() const {
    CALL_GL(glUseProgram(0))
}

uint32_t Shader::compileShader(uint32_t type, const std::string& source) {
    uint32_t id = glCreateShader(type);

    const char* raw_source = source.c_str();
    CALL_GL(glShaderSource(id, 1, &raw_source, nullptr));
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

int32_t Shader::getUniformLocation(const std::string& name) {
    try {
        auto location = uniform_map.at(name);
        return location;
    } catch (std::out_of_range e) {
        int32_t location = glGetUniformLocation(renderer_id, name.c_str());
        if (location == -1){
            std::println(std::cerr, "[WARNING] uniform '{}' does not exist.", name);
        }
        uniform_map[name] = location;
        return location;
    };
}
