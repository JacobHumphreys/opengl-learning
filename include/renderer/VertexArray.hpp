#pragma once

#include "renderer/VertexBuffer.hpp"
#include "renderer/VertexBufferLayout.hpp"
#include <cstdint>

class VertexArray {
    uint32_t renderer_id;

public:
    VertexArray();
    ~VertexArray();

    void bind() const;
    void unbind() const;
    void addBuffer(VertexBuffer const& buffer, VertexBufferLayout const& layout);
};
