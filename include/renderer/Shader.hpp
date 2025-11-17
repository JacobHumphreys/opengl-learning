#pragma once

#include "GL/glew.h"
#include <cstdint>
#include <map>
#include <string>

struct float4 {
    float x, y, z, w;
    float &r = x, g = y, b = z, a = w;

    float4(float x, float y, float z, float a)
        : x(x)
        , y(y)
        , z(z)
        , a(a) {
    }
};

class Shader {
    uint32_t renderer_id;
    std::map<std::string, int32_t> uniform_map = {};
    // cache for uniforms

public:
    Shader(const std::string& file_path);
    ~Shader();

    void setUniform4f(const std::string& name, float4 value);

    void bind() const;
    void unbind() const;

private:
    static uint32_t compileShader(uint32_t type, const std::string& source);

    int32_t getUniformLocation(const std::string& name);
};
